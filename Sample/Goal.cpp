#include "Goal.h"

#include "Graphics/ResourceManager.h"

void Goal::Initialize() {
    SetName("Goal");

    transform.UpdateMatrix();

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Goal"));
    model_->SetOutlineColor({ 1.0f,0.0f,1.0f });
    collider_ = std::make_unique<BoxCollider>();
    collider_->SetGameObject(this);
    collider_->SetName("Goal");

    Vector3 colliderTranslate = transform.worldMatrix.GetTranslate() + Vector3{ 0.0f, 1.5f, 0.0f };
    collider_->SetCenter(colliderTranslate);
    collider_->SetOrientation(transform.rotate);
    collider_->SetSize({ 1.0f, 3.0f, 1.0f });
}

void Goal::Update() {
    transform.UpdateMatrix();
    Vector3 colliderTranslate = transform.worldMatrix.GetTranslate() + Vector3{ 0.0f, 1.5f, 0.0f };
    collider_->SetCenter(colliderTranslate);
    collider_->SetOrientation(transform.rotate);
    collider_->SetSize({ 1.0f, 3.0f, 1.0f });
    model_->SetWorldMatrix(transform.worldMatrix);
}
