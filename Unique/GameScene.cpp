#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"

void GameScene::OnInitialize() {
    camera_ = std::make_shared<Camera>();
    camera_->SetPosition(camera_->GetPosition() + Vector3{ 0.0f, 3.0f, -2.0f });
    camera_->UpdateMatrices();
    RenderManager::GetInstance()->SetCamera(camera_);

    sunLight_ = std::make_shared<DirectionalLight>();
    RenderManager::GetInstance()->SetSunLight(sunLight_);

    floor_ = Model::Load("Resources/Floor/Floor.obj");
    teapot_ = Model::Load("Resources/Teapot/teapot.obj");
    bunny_ = Model::Load("Resources/Bunny/bunny.obj");
    box_ = Model::Load("Resources/box.obj");
    cone_ = Model::Load("Resources/cone.obj");
    cylinder_ = Model::Load("Resources/cylinder.obj");
    torus_ = Model::Load("Resources/torus.obj");
    suzanne_ = Model::Load("Resources/suzanne.obj");
    skydome_ = Model::Load("Resources/skydome.obj");

    instances_.resize(9);
    {
        size_t i = 0;
        instances_[i++].model.SetModel(floor_);
        instances_[i++].model.SetModel(teapot_);
        instances_[i++].model.SetModel(bunny_);
        instances_[i++].model.SetModel(box_);
        instances_[i++].model.SetModel(cone_);
        instances_[i++].model.SetModel(cylinder_);
        instances_[i++].model.SetModel(torus_);
        instances_[i++].model.SetModel(suzanne_);
        instances_[i++].model.SetModel(skydome_);
    }

    for (size_t i = 1; i < instances_.size() - 1; ++i) {
        instances_[i].transform.translate = { i * 5.0f - 20.0f, 5.0f, 0.0f };
    }

    instances_[8].model.SetCastShadow(false);
    instances_[8].model.SetReciveShadow(false);
    instances_[8].model.SetUseLighting(false);

    instances_[0].model.SetReflection(true);
    instances_[1].model.SetReflection(true);
    instances_[4].model.SetReflection(true);
    instances_[6].model.SetReflection(true);
}

void GameScene::OnUpdate() {

    static float ror = 0.0f;
    ror += Math::ToRadian * 0.5f;
    for (size_t i = 1; i < instances_.size() - 1; ++i) {
        instances_[i].transform.rotate = Quaternion::MakeFromEulerAngle({ 0.0f, ror, 0.0f });
    }

    for (auto& instance : instances_) {
        instance.transform.UpdateMatrix();
        instance.model.SetWorldMatrix(instance.transform.worldMatrix);
    }

    Input* input = Input::GetInstance();

    auto mouseMoveX = input->GetMouseMoveX();
    auto mouseMoveY = input->GetMouseMoveY();
    auto wheel = input->GetMouseWheel();

    Quaternion rotate = camera_->GetRotate();
    Vector3 position = camera_->GetPosition();

    Vector3 diffPosition;

    if (input->IsMousePressed(1)) {
        constexpr float rotSpeed = Math::ToRadian * 0.1f;
        euler_.x += rotSpeed * static_cast<float>(mouseMoveY);
        euler_.y += rotSpeed * static_cast<float>(mouseMoveX);
    }
    else if (input->IsMousePressed(2)) {
        Vector3 cameraX = rotate.GetRight() * (-static_cast<float>(mouseMoveX) * 0.01f);
        Vector3 cameraY = rotate.GetUp() * (static_cast<float>(mouseMoveY) * 0.01f);
        diffPosition += cameraX + cameraY;
    }
    else if (wheel != 0) {
        Vector3 cameraZ = rotate.GetForward() * (static_cast<float>(wheel / 120) * 0.5f);
        diffPosition += cameraZ;
    }


    camera_->SetPosition(position + diffPosition);
    camera_->SetRotate(Quaternion::MakeFromEulerAngle(euler_));
    camera_->UpdateMatrices();

    sunLight_->DrawImGui("SunLight");

}

void GameScene::OnFinalize() {
}
