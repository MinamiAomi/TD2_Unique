#include "CollisionManager.h"

CollisionManager* CollisionManager::GetInstance() {
    static CollisionManager instance;
    return &instance;
}

void CollisionManager::AddCollider(Collider* collider) { 
    colliders_.emplace_back(collider); 
}

void CollisionManager::RemoveCollider(Collider* collider) {
    auto iter = std::find(colliders_.begin(), colliders_.end() , collider);
    if (iter != colliders_.end()) {
        colliders_.erase(iter);
    }
}

void CollisionManager::CheckCollision() {

    for (auto& collider : colliders_) {
        collider->UpdateAABB();
    }

    auto iter1 = colliders_.begin();
    for (; iter1 != colliders_.end(); ++iter1) {
        Collider* collider1 = *iter1;
        // アクティブじゃなければ通さない
        if (!collider1->isActive_) { continue; }

        auto iter2 = iter1;
        ++iter2;
        for (; iter2 != colliders_.end(); ++iter2) {
            Collider* collider2 = *iter2;
            // アクティブじゃなければ通さない
            if (!collider2->isActive_) { continue; }

            CollisionInfo collisionInfo1;      
            if (collider1->IsCollision(collider2, collisionInfo1)) {
                // 衝突情報を反転
                CollisionInfo collisionInfo2 = collisionInfo1;
                collisionInfo2.collider = collider1;
                collisionInfo2.normal = -collisionInfo1.normal;
                // Stayを呼び出す
                collider1->OnCollision(collisionInfo1);
                collider2->OnCollision(collisionInfo2);
            }
        }
    }

}

bool CollisionManager::RayCast(const Vector3& origin, const Vector3& diff, uint32_t mask, RayCastInfo* nearest) {
    
    RayCastInfo tmpNearest{};
    tmpNearest.nearest = 1.1f;

    for (auto collider : colliders_) {
        if (!collider->isActive_) { continue; }
        
        RayCastInfo info{};
        info.nearest = FLT_MAX;
        if (collider->RayCast(origin, diff, mask, info)) {
            if (info.nearest < tmpNearest.nearest) {
                tmpNearest = info;
                tmpNearest.collider = collider;
            }
        }   
    }
    if (tmpNearest.nearest > 1.0f) {
        return false;
    }
    if (nearest) {
        *nearest = tmpNearest;
    }

    return true;
}

NearestInfo CollisionManager::NearestCollider(const Vector3& point, uint32_t mask) {

    NearestInfo nearestInfo{};
    nearestInfo.collider = nullptr;
    float maxDistance = FLT_MAX;

    for (auto collider : colliders_) {
        if (!collider->isActive_) { continue; }

        NearestInfo info{};
        info.collider = nullptr;
        collider->Nearest(point, mask, info);
        if (info.collider == nullptr) { continue; }
        
        float distance = (info.point - point).Length();
        if (distance < maxDistance) {
            maxDistance = distance;
            nearestInfo.collider = info.collider;
            nearestInfo.point = info.point;
        }
    }

    if (nearestInfo.collider) {
        nearestInfo.normal = nearestInfo.collider->CalcSurfaceNormal(nearestInfo.point);
    }
    return nearestInfo;
}
