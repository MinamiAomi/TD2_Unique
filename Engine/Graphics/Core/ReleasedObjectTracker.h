#pragma once

#include <d3d12.h>  
#include <wrl/client.h>

#include <array>
#include <vector>
#include <string>

class ReleasedObjectTracker {
public:
    static const UINT kAliveFrameCount = 3;

    void AddObject(Microsoft::WRL::ComPtr<ID3D12Object> releasedObject);
    void FrameIncrementForRelease();
    void AllRelease();

private:
    struct ReleasedObject {
        Microsoft::WRL::ComPtr<ID3D12Object> ptr;
#ifdef _DEBUG
        std::wstring name;
#endif // _DEBUG
    };

    using TrackingObjectList = std::vector<ReleasedObject>;
    
    std::array<TrackingObjectList, kAliveFrameCount> trackingObjectLists_;
};