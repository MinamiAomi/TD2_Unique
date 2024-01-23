#pragma once
#include <vector>

#include "Math/MathUtils.h"
#include "Collider.h"

class CollisionManager {
public:
    static CollisionManager* GetInstance();

    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);
    void ClearCollider() { colliders_.clear(); }

    void CheckCollision();

    bool RayCast(const Vector3& origin, const Vector3& diff, uint32_t mask, RayCastInfo* nearest);
    NearestInfo NearestCollider(const Vector3& point, uint32_t mask);

private:
    CollisionManager() = default;
    ~CollisionManager() = default;
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;
    CollisionManager(CollisionManager&&) = delete;
    CollisionManager& operator=(CollisionManager&&) = delete;

    std::vector<Collider*> colliders_;
};