#include "GPUResource.h"

#include "Graphics.h"
#include "Helper.h"

void GPUResource::Destroy() {
    if (resource_) {
        Graphics::GetInstance()->GetReleasedObjectTracker().AddObject(resource_);
        resource_ = nullptr;
    }
}

void GPUResource::CreateResource(
    const std::wstring& name,
    const D3D12_HEAP_PROPERTIES& heapProperties,
    const D3D12_RESOURCE_DESC& desc,
    D3D12_RESOURCE_STATES initState,
    const D3D12_CLEAR_VALUE* optimizedClearValue) {
    Destroy();

    ASSERT_IF_FAILED(Graphics::GetInstance()->GetDevice()->CreateCommittedResource(
        &heapProperties, 
        D3D12_HEAP_FLAG_NONE, 
        &desc, 
        initState, 
        optimizedClearValue, 
        IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())));
    state_ = initState;
    D3D12_OBJECT_SET_NAME(resource_, name.c_str());

#ifdef _DEBUG
    name_ = name;
#endif // _DEBUG

}
