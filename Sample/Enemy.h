#pragma once
#include "Collision/GameObject.h"

#include "Graphics/ToonModel.h"
#include "Collision/Collider.h"

class Enemy :
    public GameObject {
public:
    enum class Part {
        Body,
        Head,

        NumParts
    };

    void Initialize(const Vector3& basePosition);
    void Update();
    bool IsDead() const { return isDead_; }
    Vector3 GetCenter();

private:
    struct PartData {
        ToonModelInstance model;
        Transform transform;
    };
    
    void AliveUpdate();
    void DeadUpdate();
    void UpdateAnimation();
    void OnCollision(const CollisionInfo& collisionInfo);
    void HitEffect();
    void DeadEffect();
    
    std::unique_ptr<PartData> parts_[static_cast<size_t>(Part::NumParts)];
    std::unique_ptr<BoxCollider> collider_;

    uint32_t invincibleTime_;
    uint32_t hitCoolTime_;

    uint32_t life_;
    float animationParameter_;
    float alpha_;
    // 吹っ飛ぶ速度
    Vector3 leapingVelocity_;
    bool isDead_;
};