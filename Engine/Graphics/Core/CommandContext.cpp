#include "CommandContext.h"

#include <vector>

#include "Helper.h"
#include "Graphics.h"
#include "DescriptorHeap.h"


void CommandContext::Start(D3D12_COMMAND_LIST_TYPE type) {
    // 残っているはずがない
    assert(commandAllocator_ == nullptr);

    type_ = type;

    auto graphics = Graphics::GetInstance();
    device_ = graphics->GetDevice();
    auto& queue = graphics->GetCommandQueue(type_);
    commandAllocator_ = graphics->GetCommandAllocatorPool(type_).Allocate(queue.GetLastCompletedFenceValue());
    commandList_ = graphics->GetCommandListPool(type_).Allocate(commandAllocator_);
    
    for (int i = 0; i < LinearAllocatorType::Count; ++i) {
        dynamicBuffers_[i].Create(LinearAllocatorType::Type(i));
    }

    if (graphics->IsDXRSupported()) {
        ASSERT_IF_FAILED(commandList_.As(&dxrCommandList_));
    }


    resourceHeap_ = (ID3D12DescriptorHeap*)graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    samplerHeap_ = (ID3D12DescriptorHeap*)graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    ID3D12DescriptorHeap* ppHeaps[] = {
        resourceHeap_,
        samplerHeap_
    };
    commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    rootSignature_ = nullptr;
    pipelineState_ = nullptr;
    primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

    isClose_ = false;
}

void CommandContext::Close() {
    assert(!isClose_);
    // バリアをフラッシュ
    FlushResourceBarriers();
    ASSERT_IF_FAILED(commandList_->Close());
    isClose_ = true;
}

UINT64 CommandContext::Finish(bool waitForCompletion) {
    if (!isClose_) {
        Close();
    }

    auto graphics = Graphics::GetInstance();
    auto& queue = graphics->GetCommandQueue(type_);

    UINT64 fenceValue = queue.ExecuteCommandList(commandList_.Get());

    graphics->GetCommandAllocatorPool(type_).Discard(fenceValue, commandAllocator_);
    commandAllocator_ = nullptr;
    graphics->GetCommandListPool(type_).Discard(commandList_);
    commandList_ = nullptr;
    dxrCommandList_ = nullptr;
    for (int i = 0; i < LinearAllocatorType::Count; ++i) {
        dynamicBuffers_[i].Reset(type_, fenceValue);
    }

    if (waitForCompletion) {
        queue.WaitForGPU(fenceValue);
    }

    return fenceValue;
}

