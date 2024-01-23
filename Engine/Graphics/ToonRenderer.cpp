#include "ToonRenderer.h"

#include "Core/Helper.h"
#include "Core/ShaderManager.h"
#include "Core/ColorBuffer.h"
#include "Core/DepthBuffer.h"
#include "Core/CommandContext.h"
#include "Math/Camera.h"
#include "ToonModel.h"
#include "Core/SamplerManager.h"
#include "DefaultTextures.h"

const wchar_t kOutlineVertexShadedr[] = L"OutlineVS.hlsl";
const wchar_t kOutlinePixelShadedr[] = L"OutlinePS.hlsl";
const wchar_t kToonVertexShadedr[] = L"ToonVS.hlsl";
const wchar_t kToonPixelShadedr[] = L"ToonPS.hlsl";

void ToonRenderer::Initialize(const ColorBuffer& colorBuffer, const DepthBuffer& depthBuffer) {
    InitializeRootSignature();
    InitializeOutlinePass(colorBuffer.GetRTVFormat(), depthBuffer.GetFormat());
    InitializeToonPass(colorBuffer.GetRTVFormat(), depthBuffer.GetFormat());
}

void ToonRenderer::Render(CommandContext& commandContext, const Camera& camera) {

    struct SceneConstant {
        Matrix4x4 viewProjectionMatrix;
        Vector3 cameraPosition;
    };

    struct InstanceConstant {
        Matrix4x4 worldMatrix;
        Vector3 color;
        float alpha;
        float outlineWidth;
        Vector3 outlineColor;
        uint32_t useLighting;
    };

    auto& instanceList = ToonModelInstance::GetInstanceList();


    // 描画
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    SceneConstant scene{};
    scene.viewProjectionMatrix = camera.GetViewProjectionMatrix();
    scene.cameraPosition = camera.GetPosition();
    commandContext.SetDynamicConstantBufferView(ToonRootIndex::Scene, sizeof(scene), &scene);

    for (auto& instance : instanceList) {
        if (instance->IsActive() && instance->model_) {

            InstanceConstant data;
            data.worldMatrix = instance->worldMatrix_;
            data.color = instance->color_;
            data.alpha = instance->alpha_;
            data.outlineWidth = instance->outlineWidth_;
            data.outlineColor = instance->outlineColor_;
            data.useLighting = instance->useLighting_ ? 1 : 0;
            commandContext.SetDynamicConstantBufferView(ToonRootIndex::Instance, sizeof(data), &data); 

            // オブジェクト描画
            commandContext.SetPipelineState(toonPipelineState_);

            for (auto& mesh : instance->model_->meshes_) {
                commandContext.SetConstantBuffer(ToonRootIndex::Material, mesh.material->constantBuffer.GetGPUVirtualAddress());
                if (mesh.material->texture) {
                    commandContext.SetDescriptorTable(ToonRootIndex::Texture, mesh.material->texture->textureResource.GetSRV());
                }
                else {
                    commandContext.SetDescriptorTable(ToonRootIndex::Texture, DefaultTexture::White.GetSRV());
                }
                commandContext.SetDescriptorTable(ToonRootIndex::Sampler, SamplerManager::AnisotropicWrap);

                D3D12_VERTEX_BUFFER_VIEW vbv{};
                vbv.BufferLocation = mesh.vertexBuffer.GetGPUVirtualAddress();
                vbv.SizeInBytes = (UINT)mesh.vertexBuffer.GetBufferSize();
                vbv.StrideInBytes = (UINT)sizeof(ModelData::Vertex);
                commandContext.SetVertexBuffer(0, vbv);
                D3D12_INDEX_BUFFER_VIEW ibv{};
                ibv.BufferLocation = mesh.indexBuffer.GetGPUVirtualAddress();
                ibv.SizeInBytes = (UINT)mesh.indexBuffer.GetBufferSize();
                ibv.Format = (sizeof(ModelData::Index) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
                commandContext.SetIndexBuffer(ibv);
                commandContext.DrawIndexed(mesh.indexCount);
            }

            // アウトライン描画
            if (instance->useOutline_) {
                commandContext.SetPipelineState(outlinePipelineState_);

                for (auto& mesh : instance->model_->meshes_) {
                    D3D12_VERTEX_BUFFER_VIEW vbv{};
                    vbv.BufferLocation = mesh.vertexBuffer.GetGPUVirtualAddress();
                    vbv.SizeInBytes = (UINT)mesh.vertexBuffer.GetBufferSize();
                    vbv.StrideInBytes = (UINT)sizeof(ModelData::Vertex);
                    commandContext.SetVertexBuffer(0, vbv);
                    D3D12_INDEX_BUFFER_VIEW ibv{};
                    ibv.BufferLocation = mesh.indexBuffer.GetGPUVirtualAddress();
                    ibv.SizeInBytes = (UINT)mesh.indexBuffer.GetBufferSize();
                    ibv.Format = (sizeof(ModelData::Index) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
                    commandContext.SetIndexBuffer(ibv);
                    commandContext.DrawIndexed(mesh.indexCount);
                }
            }
        }
    }
}

void ToonRenderer::InitializeRootSignature() {
    CD3DX12_DESCRIPTOR_RANGE srvRange[2]{};
    srvRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    srvRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1);

    CD3DX12_DESCRIPTOR_RANGE samplerRange{};
    samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

    CD3DX12_ROOT_PARAMETER rootParameters[ToonRootIndex::NumParameters]{};
    rootParameters[ToonRootIndex::Scene].InitAsConstantBufferView(0);
    rootParameters[ToonRootIndex::Instance].InitAsConstantBufferView(1);
    rootParameters[ToonRootIndex::Material].InitAsConstantBufferView(2);
    rootParameters[ToonRootIndex::Texture].InitAsDescriptorTable(1, &srvRange[0]);
    rootParameters[ToonRootIndex::Sampler].InitAsDescriptorTable(1, &samplerRange);
    rootParameters[ToonRootIndex::DirectionalLight].InitAsConstantBufferView(3);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignature_.Create(L"Toon RootSignature", rootSignatureDesc);
}

void ToonRenderer::InitializeOutlinePass(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};

    pipelineStateDesc.pRootSignature = rootSignature_;

    D3D12_INPUT_ELEMENT_DESC inputElements[] = {
         { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
         { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    D3D12_INPUT_LAYOUT_DESC inputLayout{};
    inputLayout.NumElements = _countof(inputElements);
    inputLayout.pInputElementDescs = inputElements;
    pipelineStateDesc.InputLayout = inputLayout;

    auto shaderManager = ShaderManager::GetInstance();
    auto vs = shaderManager->Compile(kOutlineVertexShadedr, ShaderManager::kVertex);
    auto ps = shaderManager->Compile(kOutlinePixelShadedr, ShaderManager::kPixel);

    pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

    pipelineStateDesc.BlendState = Helper::BlendDisable;
    pipelineStateDesc.DepthStencilState = Helper::DepthStateReadWrite;
    pipelineStateDesc.RasterizerState = Helper::RasterizerDefault;
    // 前面カリング
    pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = rtvFormat;
    pipelineStateDesc.DSVFormat = dsvFormat;
    pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.SampleDesc.Count = 1;
    outlinePipelineState_.Create(L"Outline PipelineState", pipelineStateDesc);
}

void ToonRenderer::InitializeToonPass(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};

    pipelineStateDesc.pRootSignature = rootSignature_;

    D3D12_INPUT_ELEMENT_DESC inputElements[] = {
         { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
         { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
         { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    D3D12_INPUT_LAYOUT_DESC inputLayout{};
    inputLayout.NumElements = _countof(inputElements);
    inputLayout.pInputElementDescs = inputElements;
    pipelineStateDesc.InputLayout = inputLayout;

    auto shaderManager = ShaderManager::GetInstance();
    auto vs = shaderManager->Compile(kToonVertexShadedr, ShaderManager::kVertex);
    auto ps = shaderManager->Compile(kToonPixelShadedr, ShaderManager::kPixel);

    pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

    pipelineStateDesc.BlendState = Helper::BlendAlpha;
    pipelineStateDesc.DepthStencilState = Helper::DepthStateReadWrite;
    pipelineStateDesc.RasterizerState = Helper::RasterizerDefault;
    // 前面カリング
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = rtvFormat;
    pipelineStateDesc.DSVFormat = dsvFormat;
    pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.SampleDesc.Count = 1;
    toonPipelineState_.Create(L"Toon PipelineState", pipelineStateDesc);
}
