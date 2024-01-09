#include "DescriptorHeap.h"

#include <cassert>

#include "Graphics.h"
#include "Helper.h"

DescriptorHeap::DescriptorHeap() :
    type_(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES),
    cpuStart_(D3D12_CPU_DESCRIPTOR_HANDLE_NULL),
    gpuStart_(D3D12_GPU_DESCRIPTOR_HANDLE_NULL),
    numDescriptors_(0),
    descriptorSize_(0) {
}

void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) {
    assert(type != D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);

    auto device = Graphics::GetInstance()->GetDevice();

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.Type = type;
    desc.NumDescriptors = numDescriptors;
    if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    }
    ASSERT_IF_FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descriptorHeap_.ReleaseAndGetAddressOf())));

    type_ = type;
    cpuStart_ = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    if (desc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
        gpuStart_ = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
    }
    descriptorSize_ = device->GetDescriptorHandleIncrementSize(type_);
    numDescriptors_ = numDescriptors;

    freeList_.Resize(numDescriptors_);
}

DescriptorHandle DescriptorHeap::Allocate() {
    // もう空きがないので拡張しましょう
    assert(freeList_.FreeCount() != 0);
    // フリーリスト
    uint32_t allocationIndex = freeList_.Allocate();

    DescriptorHandle allocationHandle;
    allocationHandle.index_ = allocationIndex;
    allocationHandle.heap_ = shared_from_this();
    // CPUハンドルを計算
    allocationHandle.cpu_ = cpuStart_;
    allocationHandle.cpu_.ptr += allocationHandle.index_ * SIZE_T(descriptorSize_);
    // GPUハンドルを計算
    if (gpuStart_.ptr != 0) {
        allocationHandle.gpu_ = gpuStart_;
        allocationHandle.gpu_.ptr += allocationHandle.index_ * UINT64(descriptorSize_);
    }
    return allocationHandle;
}

void DescriptorHeap::Free(DescriptorHandle* descriptorHandle) {
    assert(descriptorHandle != nullptr);
    assert(!descriptorHandle->IsNull());
    assert(descriptorHandle->heap_.lock() == shared_from_this());
    freeList_.Free(descriptorHandle->index_);
    descriptorHandle->cpu_ = D3D12_CPU_DESCRIPTOR_HANDLE_NULL;
    descriptorHandle->gpu_ = D3D12_GPU_DESCRIPTOR_HANDLE_NULL;
    descriptorHandle->index_ = 0;
    descriptorHandle->heap_.reset();
}
