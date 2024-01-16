#include "BLAS.h"

#include "../Core/Graphics.h"
#include "../Core/CommandContext.h"
#include "../Core/GPUBuffer.h"

void BLAS::Create(const std::wstring& name, CommandContext& commandContext, const std::vector<Mesh>& meshes) {

    auto graphics = Graphics::GetInstance();

    std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDescs;
    geometryDescs.reserve(meshes.size());
    for (auto& mesh : meshes) {
        auto& geometryDesc = geometryDescs.emplace_back();

        // 三角形
        geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        // 不透明
        geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        // メッシュをセット
        geometryDesc.Triangles.VertexBuffer.StartAddress = mesh.vertexBuffer.GetGPUVirtualAddress();;
        geometryDesc.Triangles.VertexBuffer.StrideInBytes = mesh.vertexBuffer.GetElementSize();
        geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        geometryDesc.Triangles.VertexCount = mesh.vertexBuffer.GetNumElements();
        geometryDesc.Triangles.IndexBuffer = mesh.indexBuffer.GetGPUVirtualAddress();
        geometryDesc.Triangles.IndexFormat = mesh.indexBuffer.GetElementSize() == sizeof(UINT) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
        geometryDesc.Triangles.IndexCount = mesh.indexBuffer.GetNumElements();
    }

    // ASビルド時に必要な情報
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS asInputs{};
    asInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    asInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
    // 容量を小さく
    asInputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
    // トレースを早く
    asInputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    asInputs.NumDescs = UINT(geometryDescs.size());
    asInputs.pGeometryDescs = geometryDescs.data();
    asInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

    // ASのサイズを取得
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO asInfo{};
    graphics->GetDXRDevoce()->GetRaytracingAccelerationStructurePrebuildInfo(&asInputs, &asInfo);

    CD3DX12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resultDesc = CD3DX12_RESOURCE_DESC::Buffer(asInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    CreateResource(name, defaultHeapProps, resultDesc, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
    // スクラッチリソース一時的なリソース
    GPUResource scratchResource;
    CD3DX12_RESOURCE_DESC scratchDesc = CD3DX12_RESOURCE_DESC::Buffer(asInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    scratchResource.CreateResource(L"BLAS ScratchDataBuffer", defaultHeapProps, scratchDesc, D3D12_RESOURCE_STATE_COMMON);

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
    asDesc.Inputs = asInputs;
    asDesc.DestAccelerationStructureData = resource_->GetGPUVirtualAddress();
    asDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();

    commandContext.GetDXRCommandList()->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);
    // 生成完了までUAVバリアを張る
    commandContext.UAVBarrier(*this);
    commandContext.FlushResourceBarriers();
}
