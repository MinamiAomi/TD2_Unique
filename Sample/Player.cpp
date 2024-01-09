#include "Player.h"

#include <cassert>

#include "Input/Input.h"
#include "Graphics/ResourceManager.h"
#include "CollisionAttribute.h"
#include "Collision/CollisionManager.h"

#include "Graphics/ImGuiManager.h"

#include "Weapon.h"
#include "GlobalVariables.h"

void Player::Initialize() {
    SetName("Player");

    constantData_.colliderOffset = { 0.0f, 1.0f, 0.0f };
    constantData_.moveSpeed = 0.3f;
    constantData_.gravity = 0.02f;;
    constantData_.jumpPower = 0.3f;
    constantData_.maxFallSpeed = 0.5f;
    constantData_.dushTime = 10;
    constantData_.dushSpeed = 0.8f;
    
    constantData_.swingTimes[0] = 10;
    constantData_.swingTimes[1] = 10;
    constantData_.swingTimes[2] = 30;

    constantData_.recoveryTimes[0] = 20;
    constantData_.recoveryTimes[1] = 20;
    constantData_.recoveryTimes[2] = 30;


    transform.translate = Vector3::zero;
    transform.scale = Vector3::one;

    model_ = std::make_unique<ToonModelInstance>();
    model_->SetModel(ResourceManager::GetInstance()->FindModel("Player"));
    model_->SetIsActive(true);
    //model_->SetUseOutline(false);
    model_->SetOutlineWidth(0.03f);
    model_->SetOutlineColor({ 0.0f,0.0f,0.0f });

    collider_ = std::make_unique<BoxCollider>();
    collider_->SetGameObject(this);
    collider_->SetName("Player");
    collider_->SetCenter(transform.translate + constantData_.colliderOffset);
    collider_->SetSize({ 1.0f, 2.0f, 1.0f });
    collider_->SetCallback([this](const CollisionInfo& collisionInfo) { OnCollision(collisionInfo); });
    collider_->SetCollisionAttribute(CollisionAttribute::Player);
    collider_->SetCollisionMask(~CollisionAttribute::Player);
    //collider_->SetIsActive(false);


    state_ = std::make_unique<PlayerStateManager>(*this);
    state_->ChangeState<PlayerStateRoot>();

    weapon_ = std::make_shared<Weapon>();
    weapon_->Initialize();
    //weapon_->SetIsShowing(false);
    weapon_->transform.SetParent(&transform);

    RegisterGlobalVariables();
}

void Player::Update() {
    ApplyGlobalVariables();

    state_->Update();

    if (transform.worldMatrix.GetTranslate().y < -10.0f) {
        requestRestart_ = true;
    }

    UpdateTransform();
    weapon_->Update();

}

void Player::Restart() {
    transform.SetParent(nullptr);
    transform.translate = Vector3::zero;
    transform.rotate = Quaternion::identity;
    requestRestart_ = false;
    state_->ChangeState<PlayerStateRoot>();
}

void Player::UpdateTransform() {

    transform.UpdateMatrix();
    Vector3 scale, translate;
    Quaternion rotate;
    transform.worldMatrix.GetAffineValue(scale, rotate, translate);
    collider_->SetCenter(translate + constantData_.colliderOffset);
    collider_->SetOrientation(rotate);
    model_->SetWorldMatrix(transform.worldMatrix);
    weapon_->UpdateTransform();
}

void Player::OnCollision(const CollisionInfo& collisionInfo) {
    state_->OnCollision(collisionInfo);

    if (collisionInfo.collider->GetName() == "Floor") {
        // ワールド空間の押し出しベクトル
        Vector3 pushVector = collisionInfo.normal * collisionInfo.depth;
        auto parent = transform.GetParent();
        if (parent) {
            pushVector = parent->rotate.Inverse() * pushVector;
        }
        transform.translate += pushVector;

        UpdateTransform();

        const GameObject* nextParent = collisionInfo.collider->GetGameObject();
        if (nextParent) {
            transform.SetParent(&nextParent->transform);
        }
    }
    else if (collisionInfo.collider->GetName() == "Goal") {
        requestRestart_ = true;
    }
    else if (collisionInfo.collider->GetName() == "Enemy") {
        requestRestart_ = true;
    }
}

constexpr char kGroupName[] = "Player";

void Player::RegisterGlobalVariables() {
    GlobalVariables& globalVariables = *GlobalVariables::GetInstance();
    if (!globalVariables.HasGroup(kGroupName)) {
        auto& group = globalVariables[kGroupName];
        group["Dush Speed"] = constantData_.dushSpeed;
        group["Dush Time"] = int32_t(constantData_.dushTime);
        group["Swing Time 0"] = int32_t(constantData_.swingTimes[0]);
        group["Recovery Time 0"] = int32_t(constantData_.recoveryTimes[0]);
        group["Swing Time 1"] = int32_t(constantData_.swingTimes[1]);
        group["Recovery Time 1"] = int32_t(constantData_.recoveryTimes[1]);
        group["Swing Time 2"] = int32_t(constantData_.swingTimes[2]);
        group["Recovery Time 2"] = int32_t(constantData_.recoveryTimes[2]);
    }
}

void Player::ApplyGlobalVariables() {
    GlobalVariables& globalVariables = *GlobalVariables::GetInstance();
    auto& group = globalVariables[kGroupName];
    constantData_.dushSpeed = group["Dush Speed"].Get<float>();
    constantData_.dushTime = uint32_t(group["Dush Time"].Get<int32_t>());
    constantData_.swingTimes[0] = uint32_t(group["Swing Time 0"].Get<int32_t>());
    constantData_.recoveryTimes[0] = uint32_t(group["Recovery Time 0"].Get<int32_t>());
    constantData_.swingTimes[1] = uint32_t(group["Swing Time 1"].Get<int32_t>());
    constantData_.recoveryTimes[1] = uint32_t(group["Recovery Time 1"].Get<int32_t>());
    constantData_.swingTimes[2] = uint32_t(group["Swing Time 2"].Get<int32_t>());
    constantData_.recoveryTimes[2] = uint32_t(group["Recovery Time 2"].Get<int32_t>());
}