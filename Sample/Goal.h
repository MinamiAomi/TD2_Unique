#pragma once
#include "Collision/GameObject.h"

#include <memory>

#include "Graphics/ToonModel.h"
#include "Collision/Collider.h"

class Goal :
    public GameObject {
public:
    void Initialize();
    void Update();

private:
    std::unique_ptr<BoxCollider> collider_;
    std::unique_ptr<ToonModelInstance> model_;
};