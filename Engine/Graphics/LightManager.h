#pragma once

#include <string>

#include "Math/MathUtils.h"

class BaseLight {
public:
    BaseLight() : isActive_(true) {}
    virtual ~BaseLight() {}
    virtual void DrawImGui(const std::string& label) = 0;

    void SetIsActive(bool isActive) { isActive_ = isActive; }
    bool IsActive() const { return isActive_; }

protected:
    bool isActive_;
};

class DirectionalLight :
    public BaseLight {
public:
    DirectionalLight() : color(Vector3::one), direction(Vector3::down), intensity(1.0f) {}
    void DrawImGui(const std::string& label) override;

    Vector3 color;
    Vector3 direction;
    float intensity;
};