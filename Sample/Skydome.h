#pragma once
#include <memory>

#include "Math/Transform.h"
#include "Graphics/ToonModel.h"

class Skydome {
public:
    void Initialize();
    void Update();

private:
    std::shared_ptr<Transform> transform_;
    std::unique_ptr<ToonModelInstance> model_;
};