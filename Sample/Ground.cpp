#include "Ground.h"

#include "Graphics/ResourceManager.h"

void Ground::Initialize() {
    transform_ = std::make_shared<Transform>();
    transform_->translate = Vector3::zero;
    transform_->scale = Vector3::one;

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Ground"));
    model_->SetIsActive(true);
    model_->SetUseLighting(false);
    model_->SetUseOutline(false);
}

void Ground::Update() {
    transform_->UpdateMatrix();
    model_->SetWorldMatrix(transform_->worldMatrix);
}
