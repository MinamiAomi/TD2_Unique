#pragma once
#include "GPUResource.h"

#include <d3d12.h>
#include <wrl/client.h>

#include <vector>
#include <filesystem>

#include "DescriptorHandle.h"

class CommandContext;

class TextureResource : public GPUResource {
public:
    void CreateFromWICFile(const std::filesystem::path& path);
    void CreateFromWICFile(CommandContext& commandContext, const std::filesystem::path& path);
    void Create(size_t rowPitchBytes, size_t width, size_t heigh, DXGI_FORMAT format, void* dataBegin);
    void Create(CommandContext& commandContext, size_t rowPitchBytes, size_t width, size_t heigh, DXGI_FORMAT format, void* dataBegin);

    const D3D12_RESOURCE_DESC& GetDesc() const { return desc_; }
    const DescriptorHandle& GetSRV() const { return srvHandle_; }

private:
    void CreateView();
    void UploadResource(CommandContext& commandContext, size_t numSubresources, const D3D12_SUBRESOURCE_DATA* subresources);

    D3D12_RESOURCE_DESC desc_{};
    DescriptorHandle srvHandle_;
};