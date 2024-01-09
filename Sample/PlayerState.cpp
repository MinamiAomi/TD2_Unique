#include "PlayerState.h"

#include "Player.h"
#include "Input/Input.h"

void PlayerStateManager::Update() {
    if (standbyState_) {
        activeState_ = std::move(standbyState_);
        activeState_->Initialize();
    }

    if (activeState_) {
        activeState_->Update();
    }
}

void PlayerStateManager::OnCollision(const CollisionInfo& collisionInfo) {
    if (activeState_) {
        activeState_->OnCollision(collisionInfo);
    }
}

void PlayerStateRoot::Initialize() {
    ySpeed_ = 0.0f;
    manager_.player.GetWeapon()->SetIsShowing(false);
}

void PlayerStateRoot::Update() {
    const auto& constantData = manager_.player.GetConstantData();
    auto& transform = manager_.player.transform;
    auto& camera = manager_.player.GetCamera()->GetCamera();
    auto input = Input::GetInstance();

    Vector3 move{};
    // Gamepad入力
    {
        auto& xinputState = input->GetXInputState();
        const float margin = 0.8f;
        const float shortMaxReci = 1.0f / float(SHRT_MAX);
        move = { float(xinputState.Gamepad.sThumbLX), 0.0f, float(xinputState.Gamepad.sThumbLY) };
        move *= shortMaxReci;
        if (move.Length() < margin) {
            move = Vector3::zero;
        }
    }

    // 移動処理
    {
        if (move != Vector3::zero) {
            move = move.Normalized();
            // 地面に水平なカメラの回転
            move = camera->GetRotate() * move;
            move.y = 0.0f;
            move = move.Normalized() * constantData.moveSpeed;

            // 親がいる場合親の空間にする
            const Transform* parent = transform.GetParent();
            if (parent) {
                move = parent->worldMatrix.Inverse().ApplyRotation(move);
            }

            // 移動
            transform.translate += move;
            // 回転
            //transform.rotate = Quaternion::Slerp(0.2f, transform.rotate, Quaternion::MakeLookRotation(move));

            move = transform.rotate.Conjugate() * move;
            Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
            transform.rotate = Quaternion::Slerp(0.2f, Quaternion::identity, diff) * transform.rotate;
        }
    }


    ySpeed_ -= constantData.gravity;

    auto& xinputState = input->GetXInputState();
    if (canJump_ && (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
        canJump_ = false;
        ySpeed_ += constantData.jumpPower;
    }

    ySpeed_ = std::max(ySpeed_, -constantData.maxFallSpeed);
    transform.translate.y += ySpeed_;

    auto& preXInputState = input->GetPreXInputState();
    // 攻撃に遷移
    if ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
        manager_.ChangeState<PlayerStateAttack>();
    }
    // ダッシュに遷移
    if ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_X) && !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_X)) {
        manager_.ChangeState<PlayerStateDush>();
    }
}

void PlayerStateRoot::OnCollision(const CollisionInfo& collisionInfo) {
    if (collisionInfo.collider->GetName() == "Floor") {
        // 衝突位置の法線
        float dot = Dot(collisionInfo.normal, Vector3::up);
        // 地面と見なす角度
        const float kGroundGradientAngle = 45.0f * Math::ToRadian;
        if (std::abs(std::acos(dot)) < kGroundGradientAngle) {
            canJump_ = true;
            ySpeed_ = 0.0f;
        }
    }
}

void PlayerStateAttack::Initialize() {
    attackParameter_ = 0;
    comboIndex_ = 0;
    comboNext_ = false;
    manager_.player.GetWeapon()->SetIsShowing(true);
}

void PlayerStateAttack::Update() {
    auto input = Input::GetInstance();

    auto& gamepad = input->GetXInputState();
    auto& preGamepad = input->GetPreXInputState();

    if (comboIndex_ < Player::kNumCombos - 1) {
        if ((gamepad.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preGamepad.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
            comboNext_ = true;
        }
    }

    auto& constantData = manager_.player.GetConstantData();
    uint32_t comboTime = constantData.swingTimes[comboIndex_] + constantData.recoveryTimes[comboIndex_];
    if (++attackParameter_ >= comboTime) {
        if (comboNext_) {
            comboNext_ = false;

            attackParameter_ = 0;
            ++comboIndex_;

        }
        else {
            manager_.ChangeState<PlayerStateRoot>();
            return;
        }
    }

    auto& weaponTransform = manager_.player.GetWeapon()->transform;

    switch (comboIndex_) {
    case 0:
    default:
    {
        uint32_t swingTime = constantData.swingTimes[0];
        //uint32_t recoveryTime = kConstantAttacks[0].recoveryTime;

        if (attackParameter_ < swingTime) {
            float t = float(attackParameter_) / float(swingTime);
            weaponTransform.rotate = Quaternion::MakeForYAxis(Math::Lerp(t, -90.0f, 130.0f) * Math::ToRadian) * Quaternion::MakeForXAxis(Math::HalfPi);
        }
       /* else {
            float t = float(attackParameter_ - swingTime) / float(recoveryTime);
            weaponTransform.rotate = Quaternion::Slerp(t, Quaternion::MakeForXAxis(90.0f * Math::ToRadian), Quaternion::MakeForZAxis(90.0f * Math::ToRadian));
        }*/
        break;
    }
    case 1:
    {
        uint32_t swingTime = constantData.swingTimes[1];
        //uint32_t recoveryTime = kConstantAttacks[1].recoveryTime;

        if (attackParameter_ < swingTime) {
            float t = float(attackParameter_) / float(swingTime);
            weaponTransform.rotate = Quaternion::MakeForYAxis(Math::Lerp(t, 130.0f, -130.0f) * Math::ToRadian) * Quaternion::MakeForXAxis(Math::HalfPi);
        }
      /*  else {
            float t = float(attackParameter_ - swingTime) / float(recoveryTime);
            weaponTransform.rotate = Quaternion::Slerp(t, Quaternion::MakeForZAxis(-90.0f * Math::ToRadian), Quaternion::MakeFromAngleAxis(45.0f * Math::ToRadian, Vector3(-1.0f, 1.0f, 0.0f).Normalized()));
        }*/
    break;
    }
    case 2: {
        uint32_t swingTime = constantData.swingTimes[2];
        //uint32_t recoveryTime = kConstantAttacks[2].recoveryTime;

        if (attackParameter_ < swingTime) {
            float t = float(attackParameter_) / float(swingTime);
            weaponTransform.rotate = Quaternion::MakeForYAxis(Math::Lerp(t, -130.0f, 490.0f) * Math::ToRadian) * Quaternion::MakeForXAxis(Math::HalfPi);
        }
       /* else {
            float t = float(attackParameter_ - swingTime) / float(recoveryTime);
            weaponTransform.rotate = Quaternion::Slerp(t, Quaternion::MakeForZAxis(-90.0f * Math::ToRadian), Quaternion::identity);
        }*/
        break;
    }
    }

}

void PlayerStateAttack::OnCollision(const CollisionInfo& collisionInfo) {
    collisionInfo;
}

void PlayerStateDush::Initialize() {
    dushParameter_ = 0;
}

void PlayerStateDush::Update() {
    auto& transform = manager_.player.transform;
    float dushSpeed = manager_.player.GetConstantData().dushSpeed;
    auto move = transform.rotate.GetForward() * dushSpeed;
    transform.translate += move;

    uint32_t dushTime = manager_.player.GetConstantData().dushTime;
    if (++dushParameter_ >= dushTime) {
        manager_.ChangeState<PlayerStateRoot>();
    }
}

void PlayerStateDush::OnCollision(const CollisionInfo& collisionInfo) {
    collisionInfo;
}
