#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <string>

class RootSignature {
public:
    ~RootSignature() { Destroy(); }
    void Create(const std::wstring& name, const D3D12_ROOT_SIGNATURE_DESC& desc);

    operator ID3D12RootSignature* () const { return rootSignature_.Get(); }
    operator bool() const { return rootSignature_; }

    const Microsoft::WRL::ComPtr<ID3D12RootSignature>& Get() const { return rootSignature_; }

private:
    void Destroy();

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

#ifdef _DEBUG
    std::wstring name_;
#endif // _DEBUG
};