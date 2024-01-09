#include "Weapon.h"

#include "Graphics/ResourceManager.h"
#include "CollisionAttribute.h"
#include "Collision/CollisionManager.h"


void Weapon::Initialize() {
    SetName("Weapon");

    transform.translate = { 0.0f,0.5f,0.0f };
    transform.scale = Vector3::one;
    transform.UpdateMatrix();

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Weapon"));
    model_->SetIsActive(true);
    model_->SetOutlineWidth(0.02f);
    model_->SetOutlineColor({ 0.0f,0.0f,0.0f });

    collider_ = std::make_unique<BoxCollider>();
    collider_->SetGameObject(this);
    collider_->SetName("Weapon");
    collider_->SetCenter(transform.translate + colliderOffset_);
    collider_->SetSize({1.0f, 4.0f, 1.0f});
    collider_->SetCollisionAttribute(CollisionAttribute::Player);
    collider_->SetCollisionMask(~CollisionAttribute::Player);
}

void Weapon::Update() {
    UpdateTransform();
}

void Weapon::UpdateTransform() {
    transform.UpdateMatrix();
    Vector3 worldColliderOffset = colliderOffset_ * transform.worldMatrix;
    collider_->SetCenter(worldColliderOffset);
    collider_->SetOrientation(transform.rotate);
    model_->SetWorldMatrix(transform.worldMatrix);
}
