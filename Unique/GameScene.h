#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<Camera> camera_;
    Vector3 euler_;
    std::shared_ptr<DirectionalLight> sunLight_;

    std::shared_ptr<Model> floor_;
    std::shared_ptr<Model> teapot_;
    std::shared_ptr<Model> bunny_;
    std::shared_ptr<Model> box_;
    std::shared_ptr<Model> cone_;
    std::shared_ptr<Model> torus_;
    std::shared_ptr<Model> cylinder_;
    std::shared_ptr<Model> suzanne_;
    std::shared_ptr<Model> skydome_;

    struct Instance {
        ModelInstance model;
        Transform transform;
    };

    std::vector<Instance> instances_;

    ModelInstance floorModel_;
    ModelInstance teapotModel_;
    ModelInstance bunnyModel_;
};
