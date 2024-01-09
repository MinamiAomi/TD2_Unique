#include "RaytracingRenderer.h"

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

static const wchar_t kRaytracingShader[] = L"Raytracing/Raytracing.hlsl";
static const wchar_t kRayGenerationName[] = L"RayGeneration";
static const wchar_t kMissName[] = L"Miss";
static const wchar_t kPrimaryRayClosestHitName[] = L"PrimaryRayClosestHit";
static const wchar_t kShadowRayClosestHitName[] = L"ShadowRayClosestHit";
static const wchar_t kPrimaryRayHitGroupName[] = L"PrimaryRayHitGroup";
static const wchar_t kShadowRayHitGroupName[] = L"ShadowRayHitGroup";

void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc) {
    std::wstringstream wstr;
    wstr << L"\n";
    wstr << L"--------------------------------------------------------------------\n";
    wstr << L"| D3D12 State Object 0x" << static_cast<const void*>(desc) << L": ";
    if (desc->Type == D3D12_STATE_OBJECT_TYPE_COLLECTION) wstr << L"Collection\n";
    if (desc->Type == D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE) wstr << L"Raytracing Pipeline\n";

    auto ExportTree = [](UINT depth, UINT numExports, const D3D12_EXPORT_DESC* exports) {
        std::wostringstream woss;
        for (UINT i = 0; i < numExports; i++) {
            woss << L"|";
            if (depth > 0) {
                for (UINT j = 0; j < 2 * depth - 1; j++) woss << L" ";
            }
            woss << L" [" << i << L"]: ";
            if (exports[i].ExportToRename) woss << exports[i].ExportToRename << L" --> ";
            woss << exports[i].Name << L"\n";
        }
        return woss.str();
    };

    for (UINT i = 0; i < desc->NumSubobjects; i++) {
        wstr << L"| [" << i << L"]: ";
        switch (desc->pSubobjects[i].Type) {
        case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
            wstr << L"Global Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
            break;
        case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
            wstr << L"Local Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
            break;
        case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
            wstr << L"Node Mask: 0x" << std::hex << std::setfill(L'0') << std::setw(8) << *static_cast<const UINT*>(desc->pSubobjects[i].pDesc) << std::setw(0) << std::dec << L"\n";
            break;
        case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
        {
            wstr << L"DXIL Library 0x";
            auto lib = static_cast<const D3D12_DXIL_LIBRARY_DESC*>(desc->pSubobjects[i].pDesc);
            wstr << lib->DXILLibrary.pShaderBytecode << L", " << lib->DXILLibrary.BytecodeLength << L" bytes\n";
            wstr << ExportTree(1, lib->NumExports, lib->pExports);
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
        {
            wstr << L"Existing Library 0x";
            auto collection = static_cast<const D3D12_EXISTING_COLLECTION_DESC*>(desc->pSubobjects[i].pDesc);
            wstr << collection->pExistingCollection << L"\n";
            wstr << ExportTree(1, collection->NumExports, collection->pExports);
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
        {
            wstr << L"Subobject to Exports Association (Subobject [";
            auto association = static_cast<const D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
            UINT index = static_cast<UINT>(association->pSubobjectToAssociate - desc->pSubobjects);
            wstr << index << L"])\n";
            for (UINT j = 0; j < association->NumExports; j++) {
                wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
            }
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
        {
            wstr << L"DXIL Subobjects to Exports Association (";
            auto association = static_cast<const D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
            wstr << association->SubobjectToAssociate << L")\n";
            for (UINT j = 0; j < association->NumExports; j++) {
                wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
            }
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
        {
            wstr << L"Raytracing Shader Config\n";
            auto config = static_cast<const D3D12_RAYTRACING_SHADER_CONFIG*>(desc->pSubobjects[i].pDesc);
            wstr << L"|  [0]: Max Payload Size: " << config->MaxPayloadSizeInBytes << L" bytes\n";
            wstr << L"|  [1]: Max Attribute Size: " << config->MaxAttributeSizeInBytes << L" bytes\n";
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
        {
            wstr << L"Raytracing Pipeline Config\n";
            auto config = static_cast<const D3D12_RAYTRACING_PIPELINE_CONFIG*>(desc->pSubobjects[i].pDesc);
            wstr << L"|  [0]: Max Recursion Depth: " << config->MaxTraceRecursionDepth << L"\n";
            break;
        }
        case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
        {
            wstr << L"Hit Group (";
            auto hitGroup = static_cast<const D3D12_HIT_GROUP_DESC*>(desc->pSubobjects[i].pDesc);
            wstr << (hitGroup->HitGroupExport ? hitGroup->HitGroupExport : L"[none]") << L")\n";
            wstr << L"|  [0]: Any Hit Import: " << (hitGroup->AnyHitShaderImport ? hitGroup->AnyHitShaderImport : L"[none]") << L"\n";
            wstr << L"|  [1]: Closest Hit Import: " << (hitGroup->ClosestHitShaderImport ? hitGroup->ClosestHitShaderImport : L"[none]") << L"\n";
            wstr << L"|  [2]: Intersection Import: " << (hitGroup->IntersectionShaderImport ? hitGroup->IntersectionShaderImport : L"[none]") << L"\n";
            break;
        }
        }
        wstr << L"|--------------------------------------------------------------------\n";
    }
    wstr << L"\n";
    OutputDebugStringW(wstr.str().c_str());
}

void RaytracingRenderer::Create(uint32_t width, uint32_t height) {
    CreateRootSignature();
    CreateStateObject();
    CreateShaderTables();
    shadowBuffer_.Create(L"RaytracingRenderer ResultBuffer", width, height, DXGI_FORMAT_R16_FLOAT);
    reflectionBuffer_.Create(L"RaytracingRenderer ReflectionBuffer", width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
}

void RaytracingRenderer::Render(CommandContext& commandContext, const Camera& camera, const DirectionalLight& sunLight) {
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

    commandList->SetComputeRoot32BitConstant(0, tlas_.GetSRV().GetIndex(), 0);
    commandList->SetComputeRoot32BitConstant(0, shadowBuffer_.GetUAV().GetIndex(), 1);
    commandList->SetComputeRoot32BitConstant(0, reflectionBuffer_.GetUAV().GetIndex(), 2);
    commandList->SetComputeRootConstantBufferView(1, sceneCB);

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
    commandContext.UAVBarrier(reflectionBuffer_);
}

void RaytracingRenderer::CreateRootSignature() {

    {
        CD3DX12_ROOT_PARAMETER rootParameters[2]{};
        rootParameters[0].InitAsConstants(3, 0);
        rootParameters[1].InitAsConstantBufferView(1);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
        rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        globalRootSignature_.Create(L"GlobalRootSignatures", rootSignatureDesc);
    }

    {
        CD3DX12_DESCRIPTOR_RANGE srvRange{};
        srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 1);

        CD3DX12_DESCRIPTOR_RANGE samplerRange{};
        samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 1);

        CD3DX12_ROOT_PARAMETER rootParameters[5]{};
        rootParameters[0].InitAsShaderResourceView(0, 1);
        rootParameters[1].InitAsShaderResourceView(1, 1);
        rootParameters[2].InitAsDescriptorTable(1, &srvRange);
        rootParameters[3].InitAsDescriptorTable(1, &samplerRange);
        rootParameters[4].InitAsConstantBufferView(0, 1);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        hitGroupLocalRootSignature_.Create(L"HitGroupLocalRootSignature", rootSignatureDesc);
    }
}

void RaytracingRenderer::CreateStateObject() {
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
    size_t maxPayloadSize = sizeof(uint32_t) * 2 + sizeof(float) * 3;      // 最大ペイロードサイズ
    size_t maxAttributeSize = 2 * sizeof(float);   // 最大アトリビュートサイズ
    shaderConfig->Config((UINT)maxPayloadSize, (UINT)maxAttributeSize);

    // 7.パイプラインコンフィグ
    auto pipelineConfig = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    uint32_t maxTraceRecursionDepth = 3; // 一次レイ, シャドウレイ
    pipelineConfig->Config(maxTraceRecursionDepth);

    // 8.グローバルルートシグネチャ
    auto globalRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    globalRootSignature->SetRootSignature(globalRootSignature_);

    PrintStateObjectDesc(stateObjectDesc);

    stateObject_.Create(L"RaytracingStateObject", stateObjectDesc);
}

void RaytracingRenderer::CreateShaderTables() {

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
        rayGenerationShaderTable_.Create(L"RaytracingRenderer RayGenerationShaderTable", &rayGenerationShaderRecord, 1);
    }
    // ヒットグループは毎フレーム更新
    {
        ShaderRecord missShaderRecord(identifierMap_[kMissName]);
        missShaderTable_.Create(L"RaytracingRenderer MissShaderTable", &missShaderRecord, 1);
    }
}

void RaytracingRenderer::BuildScene(CommandContext& commandContext) {
    auto& instanceList = ModelInstance::GetInstanceList();

    struct MaterialConstantData {
        Vector3 color;
        uint32_t reflection;
    };

    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instanceDescs;
    instanceDescs.reserve(instanceList.size());

    std::vector<ShaderRecord> shaderRecords;
    shaderRecords.reserve(instanceList.size() + 1);

    shaderRecords.emplace_back(identifierMap_[kShadowRayHitGroupName]);

    // レイトレで使用するオブジェクトをインスタンスデスクに登録
    for (auto& instance : instanceList) {
        if (!(instance->IsActive() && instance->GetModel())) {
            continue;
        }

        auto& desc = instanceDescs.emplace_back();

        for (uint32_t y = 0; y < 3; ++y) {
            for (uint32_t x = 0; x < 4; ++x) {
                desc.Transform[y][x] = instance->GetWorldMatrix().m[x][y];
            }
        }
        desc.InstanceID = instance->ReciveShadow() ? 1 : 0;
        desc.InstanceMask = PRIMARY_RAY_ATTRIBUTE;
        if (instance->CastShadow()) {
            desc.InstanceMask |= SHADOW_RAY_ATTRIBUTE;
        }
        desc.InstanceContributionToHitGroupIndex = (UINT)shaderRecords.size() - 1;
        desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
        desc.AccelerationStructure = instance->GetModel()->GetBLAS().GetGPUVirtualAddress();


        MaterialConstantData material;
        material.color = instance->GetColor();
        material.reflection = instance->Reflection() ? 1 : 0;

        for (auto& mesh : instance->GetModel()->GetMeshes()) {
            auto& shaderRecord = shaderRecords.emplace_back(identifierMap_[kPrimaryRayHitGroupName]);
            shaderRecord.Add(mesh.vertexBuffer.GetGPUVirtualAddress());
            shaderRecord.Add(mesh.indexBuffer.GetGPUVirtualAddress());

            if (mesh.material && mesh.material->diffuseMap) {
                shaderRecord.Add(mesh.material->diffuseMap->GetSRV().GetGPU());
            }
            else {
                shaderRecord.Add(DefaultTexture::White.GetSRV().GetGPU());
            }
            shaderRecord.Add(SamplerManager::AnisotropicWrap);

            if (mesh.material && instance->UseLighting()) {
                material.color = Vector3::Scale(material.color, mesh.material->diffuse);
            }
            D3D12_GPU_VIRTUAL_ADDRESS materialCB = commandContext.TransfarUploadBuffer(sizeof(material), &material);
            shaderRecord.Add(materialCB);
        }
    }


    hitGroupShaderTable_.Create(L"RaytracingRenderer HitGroupShaderTable", shaderRecords.data(), (UINT)shaderRecords.size());
    tlas_.Create(L"RaytracingRenderer TLAS", commandContext, instanceDescs.data(), instanceDescs.size());
}
