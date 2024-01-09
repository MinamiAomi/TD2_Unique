#pragma once

#include <memory>

#include "Math/Camera.h"
#include "Graphics/Sprite.h"

class Enemy;

class LookOn {
public:
    void Initialize();
    void Update(const std::vector<std::shared_ptr<Enemy>>& enemies, const Camera& camera);
    void Restart();
    bool IsEnabled() const { return target_ ? true : false; }
    Vector3 GetTargetPosition() const;
private:
    bool IsOutRange(const Camera& camera);
    void SearchTarget(const std::vector<std::shared_ptr<Enemy>>& enemies, const Camera& camera);

    void RegisterGlobalVariables();
    void ApplyGlobalVariables();

    std::unique_ptr<Sprite> lookOnMark_;
    std::shared_ptr<Enemy> target_;

    float minDistance_ = 5.0f;
    float maxDistance_ = 40.0f;
    float angleRange_ = 20.0f * Math::ToRadian;
};