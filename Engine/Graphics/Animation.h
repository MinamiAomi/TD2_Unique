#pragma once

#include <vector>

#include "Math/MathUtils.h"
#include "assimp/scene.h"

struct KeyPosition {
    Vector3 position;
    float timeStamp;
};

struct KeyRotate {
    Quaternion rotate;
    float timeStamp;
};

struct KeyScale {
    Vector3 scale;
    float timeStamp;
};

class AnimationNode {
public:


    size_t GetPositionIndex(float animationTime);
    size_t GetRotateIndex(float animationTime);
    size_t GetScaleIndex(float animationTime);
private:
    float GetScaleFactor(float prevTimeStamp, float nextTimeStamp, float animationTime);

    std::vector<KeyPosition> positions_;
    std::vector<KeyRotate> rotates_;
    std::vector<KeyScale> scales_;

    Matrix4x4 localMatrix_;
    std::string name_;
    int32_t id_;
};