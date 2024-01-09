#include "RootSignature.h"

#include <cassert>

#include "Graphics.h"
#include "Helper.h"

void RootSignature::Create(const std::wstring& name, const D3D12_ROOT_SIGNATURE_DESC& desc) {
    Destroy();

    Microsoft::WRL::ComPtr<ID3DBlob> blob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

   /* auto d = desc;
    d.Flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;*/

    if (FAILED(D3D12SerializeRootSignature(
        &desc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        blob.GetAddressOf(),
        errorBlob.GetAddressOf()))) {
        MessageBoxA(nullptr, static_cast<char*>(errorBlob->GetBufferPointer()), "Failed create RootSignature!!", S_OK);
        OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    ASSERT_IF_FAILED(Graphics::GetInstance()->GetDevice()->CreateRootSignature(
        0,
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        IID_PPV_ARGS(rootSignature_.ReleaseAndGetAddressOf())));
    D3D12_OBJECT_SET_NAME(rootSignature_, name.c_str());
#ifdef _DEBUG
    name_ = name;
#endif // _DEBUG
}

void RootSignature::Destroy() {
    if (rootSignature_) {
        Graphics::GetInstance()->GetReleasedObjectTracker().AddObject(rootSignature_);
        rootSignature_ = nullptr;
    }
}
