#include "Floor.h"

#include "Graphics/ResourceManager.h"
#include "CollisionAttribute.h"
void Floor::Initialize(const Vector3& basePosition, float radian, float movement, uint32_t moveParamCycle) {
    SetName("Floor");
    model_ = std::make_unique<ToonModelInstance>();
    collider_ = std::make_unique<BoxCollider>();

    model_->SetModel(ResourceManager::GetInstance()->FindModel("Floor"));
    model_->SetIsActive(true);
    model_->SetUseOutline(false);


    basePosition_ = basePosition;
    transform.translate = basePosition;
    transform.rotate = Quaternion::MakeForYAxis(radian);
    transform.scale = Vector3::one;
    transform.UpdateMatrix();

    collider_->SetName("Floor");
    collider_->SetGameObject(this);
    collider_->SetOrientation(transform.rotate);
    collider_->SetSize({ 5.0f, 2.0f, 5.0f });
    collider_->SetCenter(transform.translate);
    collider_->SetCollisionAttribute(CollisionAttribute::Floor);
    collider_->SetCollisionMask(~CollisionAttribute::Floor);

    movement_ = movement;
    moveParamCycle_ = moveParamCycle;

    moveParam_ = 0.0f;
}

void Floor::Update() {
    if (moveParamCycle_ > 0) {
        const float delta = Math::TwoPi / moveParamCycle_;
        moveParam_ += delta;
        moveParam_ = std::fmod(moveParam_, Math::TwoPi);
        Vector3 move = transform.rotate * Vector3::forward * movement_ * std::sin(moveParam_);

        transform.translate = basePosition_ + move;
    }

    // 当たり判定、描画を更新
    transform.UpdateMatrix();
    collider_->SetCenter(transform.translate);
    model_->SetWorldMatrix(transform.worldMatrix);
}
