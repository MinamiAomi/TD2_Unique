#include "FollowCamera.h"

#include <cassert>

#include "Graphics/RenderManager.h"
#include "Input/Input.h"

#include "GlobalVariables.h"
#include "LookOn.h"

void FollowCamera::Initialize() {
    camera_ = std::make_shared<Camera>();
    RenderManager::GetInstance()->SetCamera(camera_);

    transform_ = std::make_shared<Transform>();
    transform_->rotate = Quaternion::MakeForXAxis(10.0f * Math::ToRadian);

    followDelay_ = 0.8f;

    RegisterGlobalVariables();
}

void FollowCamera::Update() {
    ApplyGlobalVariables();

    auto input = Input::GetInstance();
    XINPUT_STATE xinputState = input->GetXInputState();

    if (lookOn_->IsEnabled()) {
        Vector3 lookOnPosition = lookOn_->GetTargetPosition();
        Vector3 diff = lookOnPosition - interTarget_;
        diff.y = 0.0f;
        destinationRotate_ = Quaternion::MakeLookRotation(diff);
    }
    else {

        Quaternion diffRotate;
        // 左右振り向き
        if (std::abs(xinputState.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
            const float rotateSpeed = 3.0f * Math::ToRadian;
            destinationRotate_ = Quaternion::MakeForYAxis(float(xinputState.Gamepad.sThumbRX) / float(SHRT_MAX) * rotateSpeed) * destinationRotate_;
        }
    }
    transform_->rotate = Quaternion::Slerp(1.0f - followDelay_, transform_->rotate, destinationRotate_);

    if (target_) {
        interTarget_ = Vector3::Lerp(1.0f - followDelay_, interTarget_, target_->worldMatrix.GetTranslate());

        // カメラリセット
        if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
            destinationRotate_ = target_->worldMatrix.GetRotate();
        }
    }
    transform_->translate = interTarget_ + CalcOffset();


    camera_->SetPosition(transform_->translate);
    camera_->SetRotate(transform_->rotate);
    camera_->UpdateMatrices();
}

void FollowCamera::Restart() {
    destinationRotate_ = Quaternion::MakeForXAxis(10.0f * Math::ToRadian);
}

void FollowCamera::Reset() {
    if (target_) {
        interTarget_ = target_->worldMatrix.GetTranslate();
        transform_->rotate = target_->worldMatrix.GetRotate();
    }
    destinationRotate_ = transform_->rotate;
    Vector3 offset = CalcOffset();
    transform_->translate = interTarget_ + offset;
}

Vector3 FollowCamera::CalcOffset() const {
    Vector3 base = { 0.0f, 3.0f, -10.0f };
    return transform_->rotate * base;
}

constexpr char kGroupName[] = "FollowCamera";

void FollowCamera::RegisterGlobalVariables() {
    GlobalVariables& globalVariables = *GlobalVariables::GetInstance();
    if (!globalVariables.HasGroup(kGroupName)) {
        auto& group = globalVariables[kGroupName];
        group["Follow Delay"] = followDelay_;
    }
}

void FollowCamera::ApplyGlobalVariables() {
    GlobalVariables& globalVariables = *GlobalVariables::GetInstance();
    auto& group = globalVariables[kGroupName];
    followDelay_ = group["Follow Delay"].Get<float>();
}
