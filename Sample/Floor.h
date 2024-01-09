#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Math/Transform.h"
#include "Graphics/ToonModel.h"
#include "Collision/Collider.h"

class Floor :
    public GameObject {
public:
    ~Floor() {}

    void Initialize(const Vector3& basePosition, float radian, float movement, uint32_t moveParamCycle);
    void Update();

    void SetMovement(float movement) { movement_ = movement; }
    void SetMoveParamCycle(uint32_t moveParamCycle) { moveParamCycle_ = moveParamCycle; }

protected:
    std::unique_ptr<ToonModelInstance> model_;

    std::unique_ptr<BoxCollider> collider_;

    Vector3 basePosition_;
    float movement_ = 0.0f;
    float moveParam_ = 0;
    uint32_t moveParamCycle_ = 120;
};