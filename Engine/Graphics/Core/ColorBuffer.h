#pragma once
#include "PixelBuffer.h"

#include "DescriptorHandle.h"

class ColorBuffer : public PixelBuffer {
public:
    void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* resource, bool srgb = false);
    void Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format, bool srgb = false);
    void CreateArray(const std::wstring& name, uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format, bool srgb = false);

    void SetClearColor(const float* clearColor);
    const float* GetClearColor() const { return clearColor_; }
    
    DXGI_FORMAT GetRTVFormat() const { return rtvFormat_; }

    const DescriptorHandle& GetRTV() const { return rtvHandle_; }
    const DescriptorHandle& GetSRV() const { return srvHandle_; }
    const DescriptorHandle& GetUAV() const { return uavHandle_; }

private:
    void CreateViews(bool srgb);

    float clearColor_[4]{ 0.0f,0.0f,0.0f,0.0f };
    DescriptorHandle srvHandle_;
    DescriptorHandle uavHandle_;
    DescriptorHandle rtvHandle_;
    DXGI_FORMAT rtvFormat_;
};
