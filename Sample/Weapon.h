#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Math/Transform.h"
#include "Graphics/ToonModel.h"
#include "Collision/Collider.h"

class Weapon :
    public GameObject {
public:
    void Initialize();
    void Update();

    void SetIsShowing(bool isShowing) { 
        model_->SetIsActive(isShowing);
        collider_->SetIsActive(isShowing);
    }

    void UpdateTransform();

private:
    std::unique_ptr<ToonModelInstance> model_;
    std::unique_ptr<BoxCollider> collider_;

    Vector3 colliderOffset_ = { 0.0f, 2.0f, 0.0f };
};