#pragma once
#include "../Core/GPUResource.h"
#include "../Core/DescriptorHandle.h"

class StructuredBuffer;
class CommandContext;

class TLAS :
    public GPUResource {
public:
    void Create(const std::wstring& name, CommandContext& commandContext, const D3D12_RAYTRACING_INSTANCE_DESC* instanceDescs, size_t numInstanceDescs);

    const DescriptorHandle& GetSRV() const { return srvHandle_; }

private:
    void CreateView();

    DescriptorHandle srvHandle_;
    size_t reservedSize_ = 0;
};
