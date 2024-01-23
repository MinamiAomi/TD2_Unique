#include "PostEffect.h"

#include "Core/Helper.h"
#include "Core/ShaderManager.h"
#include "Core/CommandContext.h"

const wchar_t kPostEffectVertexShader[] = L"ScreenQuadVS.hlsl";
const wchar_t kPostEffectPixelShader[] = L"PostEffectPS.hlsl";
const wchar_t kPostEffectOtherPixelShader[] = L"PostEffectOtherPS.hlsl";
const wchar_t kPostEffectAddPixelShader[] = L"PostEffectAddPS.hlsl";

void PostEffect::Initialize(const ColorBuffer& target) {
    CD3DX12_DESCRIPTOR_RANGE srvRange[3]{};
    srvRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    srvRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    srvRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

    CD3DX12_ROOT_PARAMETER rootParameters[4]{};
    rootParameters[0].InitAsConstantBufferView(0);
    rootParameters[1].InitAsDescriptorTable(1, &srvRange[0]);
    rootParameters[2].InitAsDescriptorTable(1, &srvRange[1]);
    rootParameters[3].InitAsDescriptorTable(1, &srvRange[2]);

    CD3DX12_STATIC_SAMPLER_DESC staticSampler[1]{};
    staticSampler->Init(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumStaticSamplers = _countof(staticSampler);
    rootSignatureDesc.pStaticSamplers = staticSampler;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    rootSignature_.Create(L"PostEffect RootSignature", rootSignatureDesc);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
    pipelineStateDesc.pRootSignature = rootSignature_;

    auto shaderManager = ShaderManager::GetInstance();
    auto vs = shaderManager->Compile(kPostEffectVertexShader, ShaderManager::kVertex);
    auto ps = shaderManager->Compile(kPostEffectPixelShader, ShaderManager::kPixel);
    pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
    pipelineStateDesc.BlendState = Helper::BlendDisable;
    pipelineStateDesc.RasterizerState = Helper::RasterizerNoCull;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = target.GetRTVFormat();
    pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.SampleDesc.Count = 1;

    pipelineState_.Create(L"PostEffect PipelineState", pipelineStateDesc);

    ps = shaderManager->Compile(kPostEffectOtherPixelShader, ShaderManager::kPixel);
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
    pipelineStateOther_.Create(L"PostEffect PipelineState", pipelineStateDesc);

    ps = shaderManager->Compile(kPostEffectAddPixelShader, ShaderManager::kPixel);
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
    pipelineStateDesc.BlendState = Helper::BlendAdditive;
    pipelineStateAdd_.Create(L"PostEffect PipelineState", pipelineStateDesc);

    pipelineStateDesc.BlendState = Helper::BlendMultiply;
    pipelineStateMultiply_.Create(L"PostEffect PipelineState", pipelineStateDesc);
}

void PostEffect::Render(CommandContext& commandContext, ColorBuffer& texture, ColorBuffer& shadow, ColorBuffer& reflection) {
    commandContext.TransitionResource(texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(shadow, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(reflection, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(1, texture.GetSRV());
    commandContext.SetDescriptorTable(2, shadow.GetSRV());
    commandContext.SetDescriptorTable(3, reflection.GetSRV());
    commandContext.Draw(3);
}

void PostEffect::Render(CommandContext& commandContext, ColorBuffer& texture) {
    static const float cycle = 120.0f;
    static float time = 0.0f;

    time += 1.0f / cycle;

    commandContext.TransitionResource(texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineStateOther_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDynamicConstantBufferView(0, sizeof(time), &time);
    commandContext.SetDescriptorTable(1, texture.GetSRV());
    commandContext.Draw(3);
}

void PostEffect::RenderAddTexture(CommandContext& commandContext, ColorBuffer& texture) {
    commandContext.TransitionResource(texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineStateAdd_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(1, texture.GetSRV());
    commandContext.Draw(3);
}

void PostEffect::RenderMultiplyTexture(CommandContext& commandContext, ColorBuffer& texture) {
    commandContext.TransitionResource(texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineStateMultiply_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(1, texture.GetSRV());
    commandContext.Draw(3);
}
