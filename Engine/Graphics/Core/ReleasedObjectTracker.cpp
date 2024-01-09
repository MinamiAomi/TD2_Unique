#include "ReleasedObjectTracker.h"

#include <cassert>

void ReleasedObjectTracker::AddObject(Microsoft::WRL::ComPtr<ID3D12Object> releasedObject) {
    assert(releasedObject);
    auto& back = trackingObjectLists_.back();
    back.push_back(releasedObject);
}

void ReleasedObjectTracker::FrameIncrementForRelease() {
    for (size_t i = 1; i < trackingObjectLists_.size(); ++i) {
        std::swap(trackingObjectLists_[i - 1], trackingObjectLists_[i]);
    }
    trackingObjectLists_.back().clear();
}

void ReleasedObjectTracker::AllRelease() {
    for (auto& trackingObjecetList : trackingObjectLists_) {
        trackingObjecetList.clear();
    }
}
