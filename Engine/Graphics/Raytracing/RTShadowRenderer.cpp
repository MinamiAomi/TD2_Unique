#include "RTShadowRenderer.h"

#include "../Core/Graphics.h"
#include "../Core/Helper.h"
#include "../Core/ShaderManager.h"
#include "../Core/CommandContext.h"
#include "../LightManager.h"
#include "../Model.h"
#include "../DefaultTextures.h"
#include "../Core/SamplerManager.h"

#define PRIMARY_RAY_ATTRIBUTE (1 << 0)
#define SHADOW_RAY_ATTRIBUTE  (1 << 1)

static const wchar_t kRaytracingShader[] = L"Raytracing/RTShadow.hlsl";
static const wchar_t kRayGenerationName[] = L"RayGeneration";
static const wchar_t kMissName[] = L"Miss";
static const wchar_t kPrimaryRayClosestHitName[] = L"PrimaryRayClosestHit";
static const wchar_t kShadowRayClosestHitName[] = L"ShadowRayClosestHit";
static const wchar_t kPrimaryRayHitGroupName[] = L"PrimaryRayHitGroup";
static const wchar_t kShadowRayHitGroupName[] = L"ShadowRayHitGroup";

void RTShadowRenderer::Create(uint32_t width, uint32_t height) {
    CreateRootSignature();
    CreateStateObject();
    CreateShaderTables();
    shadowBuffer_.Create(L"RTShadowRenderer ShadowBuffer", width, height, DXGI_FORMAT_R16_FLOAT);
}

void RTShadowRenderer::Render(CommandContext& commandContext, const Camera& camera, const DirectionalLight& sunLight) {
    auto commandList = commandContext.GetDXRCommandList();

    // シーン定数
    struct Scene {
        Matrix4x4 viewProjectionInverseMatrix;
        Vector3 sunLightDirection;
    };
    // シーン定数を送る
    Scene scene;
    scene.viewProjectionInverseMatrix = camera.GetViewProjectionMatrix().Inverse();
    scene.sunLightDirection = sunLight.direction;
    auto sceneCB = commandContext.TransfarUploadBuffer(sizeof(scene), &scene);
    sceneCB;
    commandList;

    // TLASを生成
    BuildScene(commandContext);

    commandList->SetComputeRootSignature(globalRootSignature_);
    commandList->SetPipelineState1(stateObject_);

    commandList->SetComputeRootDescriptorTable(0, reciveShadowTlas_.GetSRV());
    commandList->SetComputeRootDescriptorTable(1, castShadowTlas_.GetSRV());
    commandList->SetComputeRootDescriptorTable(2, shadowBuffer_.GetUAV());
    commandList->SetComputeRootConstantBufferView(3, sceneCB);

    D3D12_DISPATCH_RAYS_DESC rayDesc{};
    rayDesc.RayGenerationShaderRecord.StartAddress = rayGenerationShaderTable_.GetGPUVirtualAddress();
    rayDesc.RayGenerationShaderRecord.SizeInBytes = rayGenerationShaderTable_.GetBufferSize();
    rayDesc.MissShaderTable.StartAddress = missShaderTable_.GetGPUVirtualAddress();
    rayDesc.MissShaderTable.SizeInBytes = missShaderTable_.GetBufferSize();
    rayDesc.MissShaderTable.StrideInBytes = missShaderTable_.GetShaderRecordSize();
    rayDesc.HitGroupTable.StartAddress = hitGroupShaderTable_.GetGPUVirtualAddress();
    rayDesc.HitGroupTable.SizeInBytes = hitGroupShaderTable_.GetBufferSize();
    rayDesc.HitGroupTable.StrideInBytes = hitGroupShaderTable_.GetShaderRecordSize();
    rayDesc.Width = shadowBuffer_.GetWidth();
    rayDesc.Height = shadowBuffer_.GetHeight();
    rayDesc.Depth = 1;
    commandList->DispatchRays(&rayDesc);

    commandContext.UAVBarrier(shadowBuffer_);
}

void RTShadowRenderer::CreateRootSignature() {

    {
        CD3DX12_DESCRIPTOR_RANGE ranges[3]{};
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);


        CD3DX12_ROOT_PARAMETER rootParameters[4]{};
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);
        rootParameters[1].InitAsDescriptorTable(1, &ranges[1]);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[2]);
        rootParameters[3].InitAsConstantBufferView(0);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        globalRootSignature_.Create(L"GlobalRootSignatures", rootSignatureDesc);
    }

    {
        CD3DX12_DESCRIPTOR_RANGE ranges[2]{};
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 1);

        CD3DX12_ROOT_PARAMETER rootParameters[2]{};
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);
        rootParameters[1].InitAsDescriptorTable(1, &ranges[1]);
    
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        hitGroupLocalRootSignature_.Create(L"HitGroupLocalRootSignature", rootSignatureDesc);
    }
}

void RTShadowRenderer::CreateStateObject() {
    CD3DX12_STATE_OBJECT_DESC stateObjectDesc{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

    // 1.DXILLib
    auto shader = ShaderManager::GetInstance()->Compile(kRaytracingShader, ShaderType::Library, 6, 6);
    CD3DX12_SHADER_BYTECODE shaderByteCode(shader->GetBufferPointer(), shader->GetBufferSize());
    auto dxilLibSubobject = stateObjectDesc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
    dxilLibSubobject->SetDXILLibrary(&shaderByteCode);
    dxilLibSubobject->DefineExport(kRayGenerationName);
    dxilLibSubobject->DefineExport(kMissName);
    dxilLibSubobject->DefineExport(kPrimaryRayClosestHitName);
    dxilLibSubobject->DefineExport(kShadowRayClosestHitName);

    // 2.一次レイヒットグループ
    auto primaryRayHitGroup = stateObjectDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    primaryRayHitGroup->SetClosestHitShaderImport(kPrimaryRayClosestHitName);
    primaryRayHitGroup->SetHitGroupExport(kPrimaryRayHitGroupName);
    primaryRayHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // 3.ヒットグループのローカルルートシグネチャ
    auto primaryHitGroupRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
    primaryHitGroupRootSignature->SetRootSignature(hitGroupLocalRootSignature_);

    // 4.ヒットグループアソシエーション
    auto primaryHitGroupRootSignatureAssociation = stateObjectDesc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
    primaryHitGroupRootSignatureAssociation->SetSubobjectToAssociate(*primaryHitGroupRootSignature);
    primaryHitGroupRootSignatureAssociation->AddExport(kPrimaryRayHitGroupName);

    // 5.シャドウレイヒットグループ
    auto shadowRayHitGroup = stateObjectDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    shadowRayHitGroup->SetClosestHitShaderImport(kShadowRayClosestHitName);
    shadowRayHitGroup->SetHitGroupExport(kShadowRayHitGroupName);
    shadowRayHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    // 6.シェーダーコンフィグ
    auto shaderConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    size_t maxPayloadSize = sizeof(uint32_t) * 1;      // 最大ペイロードサイズ
    size_t maxAttributeSize = 2 * sizeof(float);   // 最大アトリビュートサイズ
    shaderConfig->Config((UINT)maxPayloadSize, (UINT)maxAttributeSize);

    // 7.パイプラインコンフィグ
    auto pipelineConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    uint32_t maxTraceRecursionDepth = 2; // 一次レイ, シャドウレイ
    pipelineConfig->Config(maxTraceRecursionDepth);

    // 8.グローバルルートシグネチャ
    auto globalRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    globalRootSignature->SetRootSignature(globalRootSignature_);


    stateObject_.Create(L"RTShadowRenderer StateObject", stateObjectDesc);
}

void RTShadowRenderer::CreateShaderTables() {

    {
        Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
        stateObject_.Get().As(&stateObjectProperties);

        auto InsertIdentifier = [&](const wchar_t* name) {
            identifierMap_[name] = stateObjectProperties->GetShaderIdentifier(name);
        };
        InsertIdentifier(kRayGenerationName);
        InsertIdentifier(kPrimaryRayHitGroupName);
        InsertIdentifier(kShadowRayHitGroupName);
        InsertIdentifier(kMissName);
    }

    {
        ShaderRecord rayGenerationShaderRecord(identifierMap_[kRayGenerationName]);
        rayGenerationShaderTable_.Create(L"RTShadowRenderer RayGenerationShaderTable", &rayGenerationShaderRecord, 1);
    }
    // ヒットグループは毎フレーム更新
    {
        ShaderRecord missShaderRecord(identifierMap_[kMissName]);
        missShaderTable_.Create(L"RTShadowRenderer MissShaderTable", &missShaderRecord, 1);
    }
}

bool RTShadowRenderer::BuildScene(CommandContext& commandContext) {
    auto& instanceList = ModelInstance::GetInstanceList();

    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> reciveShadowInstanceDescs;
    reciveShadowInstanceDescs.reserve(instanceList.size());
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> castShadowInstanceDescs;
    castShadowInstanceDescs.reserve(instanceList.size());

    std::vector<ShaderRecord> shaderRecords;
    shaderRecords.reserve(instanceList.size() + 1);
    // シャドウレイヒットグループ
    shaderRecords.emplace_back(identifierMap_[kShadowRayHitGroupName]);

    // レイトレで使用するオブジェクトをインスタンスデスクに登録
    for (auto& instance : instanceList) {
        if (!(instance->IsActive() && instance->GetModel())) {
            continue;
        }

        D3D12_RAYTRACING_INSTANCE_DESC desc{};

        for (uint32_t y = 0; y < 3; ++y) {
            for (uint32_t x = 0; x < 4; ++x) {
                desc.Transform[y][x] = instance->GetWorldMatrix().m[x][y];
            }
        }
        desc.InstanceID = instance->ReciveShadow() ? 1 : 0;
        desc.InstanceMask = 0xFF;
        desc.InstanceContributionToHitGroupIndex = (UINT)shaderRecords.size() - 1;
        desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
        desc.AccelerationStructure = instance->GetModel()->GetBLAS().GetGPUVirtualAddress();

        reciveShadowInstanceDescs.emplace_back(desc);

        if (instance->CastShadow()) {
            desc.InstanceContributionToHitGroupIndex = 0;
            castShadowInstanceDescs.emplace_back(desc);
        }

        for (auto& mesh : instance->GetModel()->GetMeshes()) {
            auto& shaderRecord = shaderRecords.emplace_back(identifierMap_[kPrimaryRayHitGroupName]);
            shaderRecord.Add(mesh.vertexBuffer.GetGPUVirtualAddress());
            shaderRecord.Add(mesh.indexBuffer.GetGPUVirtualAddress());
        }
    }


    hitGroupShaderTable_.Create(L"RTShadowRenderer HitGroupShaderTable", shaderRecords.data(), (UINT)shaderRecords.size());
    if (!reciveShadowInstanceDescs.empty()) {
        reciveShadowTlas_.Create(L"RTShadowRenderer ReciveShadowTLAS", commandContext, reciveShadowInstanceDescs.data(), reciveShadowInstanceDescs.size());
    }
    if (!castShadowInstanceDescs.empty()) {
        castShadowTlas_.Create(L"RTShadowRenderer CastShadowTlas", commandContext, castShadowInstanceDescs.data(), castShadowInstanceDescs.size());
        return true;
    }
    return false;
}
