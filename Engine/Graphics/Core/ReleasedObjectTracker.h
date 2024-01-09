#pragma once

#include <d3d12.h>  
#include <wrl/client.h>

#include <array>
#include <vector>

class ReleasedObjectTracker {
public:
    static const UINT kAliveFrameCount = 2;

    void AddObject(Microsoft::WRL::ComPtr<ID3D12Object> releasedObject);
    void FrameIncrementForRelease();
    void AllRelease();

private:
    using TrackingObjectList = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;
    
    std::array<TrackingObjectList, kAliveFrameCount> trackingObjectLists_;
};