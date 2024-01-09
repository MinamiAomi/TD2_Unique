#include "LookOn.h"

#include "Graphics/ResourceManager.h"
#include "Graphics/GameWindow.h"
#include "Input/Input.h"

#include "Enemy.h"
#include "GlobalVariables.h"

void LookOn::Initialize() {

    minDistance_ = 5.0f;
    maxDistance_ = 40.0f;
    angleRange_ = 20.0f * Math::ToRadian;

    lookOnMark_ = std::make_unique<Sprite>();
    lookOnMark_->SetTexture(ResourceManager::GetInstance()->FindTexture("LookOn"));
    lookOnMark_->SetTexcoordRect({}, { 128.0f, 128.0f });
    lookOnMark_->SetScale({ 50.0f, 50.0f });

    RegisterGlobalVariables();
}

void LookOn::Update(const std::vector<std::shared_ptr<Enemy>>& enemies, const Camera& camera) {
    ApplyGlobalVariables();


    auto input = Input::GetInstance();
    auto& gamepad = input->GetXInputState();
    auto& preGamepad = input->GetPreXInputState();
    if (target_) {

        if (((gamepad.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
            !(preGamepad.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) ||
            IsOutRange(camera) ||
            target_->IsDead()) {
            target_.reset();
        }
        else if (((gamepad.Gamepad.wButtons & XINPUT_GAMEPAD_Y) &&
            !(preGamepad.Gamepad.wButtons & XINPUT_GAMEPAD_Y))) {
            SearchTarget(enemies, camera);
        }

    }
    else {
        if ((gamepad.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
            !(preGamepad.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
            SearchTarget(enemies, camera);
        }
    }


    if (target_) {
        Vector3 worldPosition = target_->GetCenter();
        // w除算まで行う
        Vector3 wDividePosition = camera.GetViewProjectionMatrix().ApplyTransformWDivide(worldPosition);

        Vector3 screenPosition = wDividePosition * Matrix4x4::MakeViewport(0, 0, float(GameWindow::GetInstance()->GetClientWidth()), float(GameWindow::GetInstance()->GetClientHeight()), 0.0f, 1.0f);
        screenPosition.y = float(GameWindow::GetInstance()->GetClientHeight()) - screenPosition.y;
        lookOnMark_->SetPosition(screenPosition.GetXY());
        lookOnMark_->SetIsActive(true);
    }
    else {
        lookOnMark_->SetIsActive(false);
    }
}

void LookOn::Restart() {
    if (target_) {
        target_.reset();
    }
}

Vector3 LookOn::GetTargetPosition() const {
    if (target_) {
        return target_->GetCenter();
    }
    return {};
}

bool LookOn::IsOutRange(const Camera& camera) {
    if (target_) {
        const float angleThreshold = std::cos(angleRange_);
        Vector3 cameraForward = camera.GetForward();
        cameraForward.y = 0.0f;
        cameraForward = cameraForward.Normalized();
        Vector3 cameraToEnemy = target_->GetCenter() - camera.GetPosition();
        cameraToEnemy.y = 0.0f;
        float distance = cameraToEnemy.Length();
        if (Dot(cameraToEnemy.Normalized(), cameraForward) > angleThreshold &&
            distance >= minDistance_ &&
            distance <= maxDistance_) {
            return false;
        }
    }

    return true;
}

void LookOn::SearchTarget(const std::vector<std::shared_ptr<Enemy>>& enemies, const Camera& camera) {
    const float angleThreshold = std::cos(angleRange_);

    // 検索
    Vector3 cameraForward = camera.GetForward();
    cameraForward.y = 0.0f;
    cameraForward = cameraForward.Normalized();

    float nearestDistance = FLT_MAX;
    std::shared_ptr<Enemy> nearestEnemy;

    for (auto& enemy : enemies) {
        if (enemy->IsDead() || target_ == enemy) {
            continue;
        }

        Vector3 cameraToEnemy = enemy->GetCenter() - camera.GetPosition();
        cameraToEnemy.y = 0.0f;

        // 角度制限
        if (Dot(cameraToEnemy.Normalized(), cameraForward) > angleThreshold) {
            float distance = cameraToEnemy.Length();
            // 距離制限
            if (distance < nearestDistance &&
                distance >= minDistance_ &&
                distance <= maxDistance_) {
                nearestEnemy = enemy;
                nearestDistance = distance;
            }
        }
    }

    target_ = nearestEnemy;
}

constexpr char kGroupName[] = "LookOn";

void LookOn::RegisterGlobalVariables() {
    GlobalVariables& globalVariables = *GlobalVariables::GetInstance();
    if (!globalVariables.HasGroup(kGroupName)) {
        auto& group = globalVariables[kGroupName];
        group["Min Distance"] = minDistance_;
        group["Max Distance"] = maxDistance_;
        group["Angle range"] = angleRange_ * Math::ToDegree;
    }
}

void LookOn::ApplyGlobalVariables() {
    GlobalVariables& globalVariables = *GlobalVariables::GetInstance();
    auto& group = globalVariables[kGroupName];
    minDistance_ =group["Min Distance"].Get<float>();
    maxDistance_ = group["Max Distance"].Get<float>();
    angleRange_ = group["Angle range"].Get<float>() * Math::ToRadian;
}
