#include "ColorBuffer.h"

#include "Graphics.h"
#include "Helper.h"

void ColorBuffer::CreateFromSwapChain(const std::wstring& name, ID3D12Resource* resource, bool srgb) {
    AssociateWithResource(name, resource, D3D12_RESOURCE_STATE_PRESENT);

    auto graphics = Graphics::GetInstance();
    if (rtvHandle_.IsNull()) {
        rtvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = srgb ? Helper::GetSRGBFormat(format_) : format_;
    graphics->GetDevice()->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvHandle_);
    rtvFormat_ = rtvDesc.Format;
}

void ColorBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format, bool srgb) {
    auto flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    auto desc = DescribeTex2D(width, height, 1, format, flags);

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format;
    memcpy(clearValue.Color, clearColor_, sizeof(clearValue.Color));

    CreateTextureResource(name, desc, clearValue);
    CreateViews(srgb);
}

void ColorBuffer::CreateArray(const std::wstring& name, uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format, bool srgb) {
    auto flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    auto desc = DescribeTex2D(width, height, arraySize, format, flags);

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format;
    memcpy(clearValue.Color, clearColor_, sizeof(clearValue.Color));

    CreateTextureResource(name, desc, clearValue);
    CreateViews(srgb);
}

void ColorBuffer::SetClearColor(const float* clearColor) {
    memcpy(clearColor_, clearColor, sizeof(clearColor_));
}

void ColorBuffer::CreateViews(bool srgb) {
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    rtvDesc.Format = srgb ? Helper::GetSRGBFormat(format_) : format_;
    srvDesc.Format = format_;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    uavDesc.Format = Helper::GetUAVFormat(format_);

    if (arraySize_ > 1) {
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc.Texture2DArray.MipSlice = 0;
        rtvDesc.Texture2DArray.FirstArraySlice = 0;
        rtvDesc.Texture2DArray.ArraySize = UINT(arraySize_);

        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MipLevels = 1;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.FirstArraySlice = 0;
        srvDesc.Texture2DArray.ArraySize = UINT(arraySize_);

        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
        uavDesc.Texture2DArray.MipSlice = 0;
        uavDesc.Texture2DArray.FirstArraySlice = 0;
        uavDesc.Texture2DArray.ArraySize = UINT(arraySize_);
    }
    else {
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;

        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = 0;

        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
    }

    auto graphics = Graphics::GetInstance();
    if (rtvHandle_.IsNull()) {
        rtvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    if (uavHandle_.IsNull()) {
        uavHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    auto device = graphics->GetDevice();
    device->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvHandle_);
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
    device->CreateUnorderedAccessView(resource_.Get(), nullptr, &uavDesc, uavHandle_);
    rtvFormat_ = rtvDesc.Format;
}
