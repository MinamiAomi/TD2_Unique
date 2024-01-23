#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "RootSignature.h"
#include "DescriptorHandle.h"
#include "DescriptorHeap.h"
#include "CommandQueue.h"
#include "CommandAllocatorPool.h"
#include "CommandListPool.h"
#include "ReleasedObjectTracker.h"
#include "LinearAllocator.h"

#define BINDLESS_RESOURCE_MAX 1024
#define DXR_DEVICE ID3D12Device5

class Graphics {
public:
    static Graphics* GetInstance();

    void Initialize();
    void Finalize();

    DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

    ID3D12Device* GetDevice() const { return device_.Get(); }
    DXR_DEVICE* GetDXRDevoce() const { return dxrDevice_.Get(); }
    CommandQueue& GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) { return GetCommandSet(type).queue; }
    CommandAllocatorPool& GetCommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type) { return GetCommandSet(type).allocatorPool; }
    CommandListPool& GetCommandListPool(D3D12_COMMAND_LIST_TYPE type) { return GetCommandSet(type).listPool; }
    DescriptorHeap& GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) { return *descriptorHeaps_[type]; }
    LinearAllocatorPagePool& GetLinearAllocatorPagePool(LinearAllocatorType type) { return linearAllocatorPagePools_[type]; }
    ReleasedObjectTracker& GetReleasedObjectTracker() { return releasedObjectTracker_; }

    RootSignature& GetDynamicResourcesRootSignature() { return dynamicResourcesRootSignature_; }

    bool IsDXRSupported() const { return dxrDevice_; }

private:
    static const uint32_t kNumRTVs = 16;
    static const uint32_t kNumDSVs = 2;
    static const uint32_t kNumSRVs = BINDLESS_RESOURCE_MAX;
    static const uint32_t kNumSamplers = 16;

    struct CommandSet {
        CommandQueue queue;
        CommandAllocatorPool allocatorPool;
        CommandListPool listPool;
        
        CommandSet(D3D12_COMMAND_LIST_TYPE type) :
            queue(type), allocatorPool(type), listPool(type) {
        }
    };

    Graphics();
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics() = default;

    void CreateDevice();
    CommandSet& GetCommandSet(D3D12_COMMAND_LIST_TYPE type);
    void CreateDynamicResourcesRootSignature();

    ReleasedObjectTracker releasedObjectTracker_;

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<DXR_DEVICE> dxrDevice_;

    CommandSet directCommandSet_;
    CommandSet computeCommandSet_;
    CommandSet copyCommandSet_;

    std::shared_ptr<DescriptorHeap> descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    LinearAllocatorPagePool linearAllocatorPagePools_[LinearAllocatorType::Count];
    
    RootSignature dynamicResourcesRootSignature_;
};

inline Graphics::CommandSet& Graphics::GetCommandSet(D3D12_COMMAND_LIST_TYPE type) {
    switch (type) {
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        return computeCommandSet_;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        return copyCommandSet_;
    }
    return directCommandSet_;
}
