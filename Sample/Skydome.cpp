#include "Skydome.h"

#include "Graphics/ResourceManager.h"

void Skydome::Initialize() {
    transform_ = std::make_shared<Transform>();
    transform_->translate = { 0.0f, -20.0f, 0.0f };
    transform_->scale = Vector3::one;

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Skydome"));
    model_->SetIsActive(true);
    model_->SetUseLighting(false);
    model_->SetUseOutline(false);
}

void Skydome::Update() {
    transform_->UpdateMatrix();
    model_->SetWorldMatrix(transform_->worldMatrix);
}
