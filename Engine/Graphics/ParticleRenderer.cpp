#include "ParticleRenderer.h"

#include "Core/Helper.h"
#include "Core/ShaderManager.h"
#include "Core/ColorBuffer.h"
#include "Core/DepthBuffer.h"
#include "Core/CommandContext.h"

#include "ParticleManager.h"

const wchar_t kParticleVertexShadedr[] = L"ParticleVS.hlsl";
const wchar_t kParticlePixelShadedr[] = L"ParticlePS.hlsl";

void ParticleRenderer::Initialize(const ColorBuffer& colorBuffer, const DepthBuffer& depthBuffer) {

    CD3DX12_ROOT_PARAMETER rootParameters[2]{};
    rootParameters[0].InitAsConstantBufferView(0);
    rootParameters[1].InitAsShaderResourceView(0);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignature_.Create(L"Particle RootSignature", rootSignatureDesc);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};

    pipelineStateDesc.pRootSignature = rootSignature_;

    D3D12_INPUT_ELEMENT_DESC inputElements[] = {
         { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
    D3D12_INPUT_LAYOUT_DESC inputLayout{};
    inputLayout.NumElements = _countof(inputElements);
    inputLayout.pInputElementDescs = inputElements;
    pipelineStateDesc.InputLayout = inputLayout;

    auto shaderManager = ShaderManager::GetInstance();
    auto vs = shaderManager->Compile(kParticleVertexShadedr, ShaderManager::kVertex);
    auto ps = shaderManager->Compile(kParticlePixelShadedr, ShaderManager::kPixel);

    pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

    pipelineStateDesc.BlendState = Helper::BlendAdditive;
    pipelineStateDesc.DepthStencilState = Helper::DepthStateReadOnly;
    pipelineStateDesc.RasterizerState = Helper::RasterizerNoCull;
    // 前面カリング
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = colorBuffer.GetRTVFormat();
    pipelineStateDesc.DSVFormat = depthBuffer.GetFormat();
    pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.SampleDesc.Count = 1;
    pipelineState_.Create(L"Particle PipelineState", pipelineStateDesc);
}

void ParticleRenderer::Render(CommandContext& commandContext, const Camera& camera) {

    auto particleManager = ParticleManager::GetInstance();
    
    particleManager->Update();

    struct Scene {
        Matrix4x4 viewProjMatrix;
    };

    struct Vertex {
        Vector3 position;
    };

    struct Instance {
        Matrix4x4 worldMatrix;
        Vector3 color;
        float alpha;
    };

    auto& particles = particleManager->GetParticles();
    // パーティクルがない
    if (particles.empty()) {
        return;
    }

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    Scene scene;
    scene.viewProjMatrix = camera.GetViewProjectionMatrix();
    commandContext.SetDynamicConstantBufferView(0, sizeof(scene), &scene);

    Vertex vertices[] = {
        { { -1.0f,  1.0f, 0.0f } },
        { {  1.0f,  1.0f, 0.0f } },
        { { -1.0f, -1.0f, 0.0f } },
        { {  1.0f, -1.0f, 0.0f } }
    };
    commandContext.SetDynamicVertexBuffer(0, _countof(vertices), sizeof(vertices[0]), vertices);


    uint32_t numParticles = 0;
    std::vector<Instance> instances(kMaxNumParticles);

    Vector3 cameraForward = camera.GetRotate().GetForward();

    for (const auto& particle : particles) {
        if (numParticles >= kMaxNumParticles) {
            break;
        }
        auto& instance = instances[numParticles];

        Vector3 direction = particle.position - camera.GetPosition();
        // カメラの向いている方向にない場合描画しない
        if (Dot(cameraForward, direction) < 0.0f) {
            continue;
        }

        float t = float(particle.existenceTime) / float(particle.lifeTime);

        instance.worldMatrix =
            Matrix4x4::MakeScaling(Vector3(Math::Lerp(t, particle.startSize, particle.endSize))) *
            Matrix4x4::MakeLookRotation(-direction) *    // カメラの方向を向ける
            Matrix4x4::MakeTranslation(particle.position);
        instance.color = Vector3::Lerp(t, particle.startColor, particle.endColor);
        instance.alpha = Math::Lerp(t, particle.startAlpha, particle.endAlpha);
    
        ++numParticles;
    }

    commandContext.SetDynamicShaderResourceView(1, sizeof(instances[0]) * numParticles, instances.data());
    commandContext.DrawInstanced(_countof(vertices), numParticles);
}
