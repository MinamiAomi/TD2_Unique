#include "Player.h"
#include "Engine/input/Input.h"
#include "Graphics/ResourceManager.h"
#include "Game/block/block.h"
#include "GlobalVariables.h"

Player::Player() {
    auto resources = ResourceManager::GetInstance();

    hpTex_ = resources->FindTexture("player_hp");

    hpSprite_ = std::make_unique<Sprite>();
    hpSprite_->SetTexture(hpTex_);
    hpSprite_->SetPosition({ 640.0f,10.0f });

    ui_A_Tex_ = resources->FindTexture("UI_A");
    ui_RB_Tex_ = resources->FindTexture("UI_RB");
    ui_LB_Tex_ = resources->FindTexture("UI_LB");
    ui_RT_Tex_ = resources->FindTexture("UI_RT");
    ui_RS_Tex_ = resources->FindTexture("UI_RS");
    ui_LS_Tex_ = resources->FindTexture("UI_LS");

    ui_A_ = std::make_unique<Sprite>();
    ui_A_->SetTexture(ui_A_Tex_);
    ui_A_->SetPosition({ 1100.0f,250.0f });
    ui_A_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
    ui_A_->SetScale({ 256.0f,64.0f });

    ui_RB_ = std::make_unique<Sprite>();
    ui_RB_->SetTexture(ui_RB_Tex_);
    ui_RB_->SetPosition({ 1050.0f,50.0f });
    ui_RB_->SetTexcoordRect({ 512.0f,64.0f }, { 512.0f,64.0f });
    ui_RB_->SetScale({ 512.0f,64.0f });

    ui_LB_ = std::make_unique<Sprite>();
    ui_LB_->SetTexture(ui_LB_Tex_);
    ui_LB_->SetPosition({ 1100.0f,350.0f });
    ui_LB_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
    ui_LB_->SetScale({ 256.0f,64.0f });

    ui_RT_ = std::make_unique<Sprite>();
    ui_RT_->SetTexture(ui_RT_Tex_);
    ui_RT_->SetPosition({ 1100.0f,150.0f });
    ui_RT_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
    ui_RT_->SetScale({ 256.0f,64.0f });

    ui_RS_ = std::make_unique<Sprite>();
    ui_RS_->SetTexture(ui_RS_Tex_);
    ui_RS_->SetPosition({ 200.0f,150.0f });
    ui_RS_->SetTexcoordRect({ 512.0f,64.0f }, { 512.0f,64.0f });
    ui_RS_->SetScale({ 384.0f,48.0f });

    ui_LS_ = std::make_unique<Sprite>();
    ui_LS_->SetTexture(ui_LS_Tex_);
    ui_LS_->SetPosition({ 200.0f,50.0f });
    ui_LS_->SetTexcoordRect({ 512.0f,64.0f }, { 512.0f,64.0f });
    ui_LS_->SetScale({ 384.0f,48.0f });

    collider_ = std::make_unique<BoxCollider>();

    shockWaveCollider_ = std::make_unique<SphereCollider>();

    weapon_ = std::make_unique<Weapon>();

    reticle_ = std::make_shared<Reticle3D>();

}

Player::~Player()
{
}

void Player::Initialize() {

    SetName("Player");

    input_ = Input::GetInstance();


    transform.scale = Vector3::one;
    transform.rotate = Quaternion::identity;
    transform.translate = { 0.0f, 2.5f, 0.0f };
    modelScaleTransform_.SetParent(&transform);
    modelScaleTransform_.scale = { 2.0f, 2.0f, 2.0f };
    modelScaleTransform_.rotate = Quaternion::identity;
    modelScaleTransform_.translate = Vector3::zero;
    model_.Initialize(&modelScaleTransform_);

    weapon_->modelBodyTransform_->SetParent(&model_.GetTransform(PlayerModel::kRightLowerArm));
    weapon_->Initialize();
    weapon_->SetPlayer(this);

    reticle_->Initialize();

    preDirection_ = { 0.0f,0.0f,1.0f };
    direction_ = { 0.0f,0.0f,1.0f };

    isDead_ = false;
    hp_ = kMaxHp_;

    collider_->SetCenter(transform.translate);
    //コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
    collider_->SetSize(transform.scale * 2.0f);
    collider_->SetName("Player");
    collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
    collider_->SetIsActive(true);
    collider_->SetGameObject(this);
    collider_->SetCollisionAttribute(0xfffffffe);
    collider_->SetCollisionMask(0x00000001);

    shockWaveCollider_->SetCenter(transform.worldMatrix.GetTranslate());
    shockWaveCollider_->SetRadius(10.0f);
    shockWaveCollider_->SetName("ShockWave");
    shockWaveCollider_->SetIsActive(false);
    shockWaveCollider_->SetGameObject(this);
    shockWaveCollider_->SetCollisionAttribute(0xfffffffe);
    shockWaveCollider_->SetCollisionMask(0x00000001);

    velocity_ = { 0.0f,0.0f,1.0f };

    behavior_ = Behavior::kRoot;

    workDash_.speed_ = 2.0f;

    attack_.attackType = kVertical;
    attack_.isCombo_ = false;

    workGravity_.gravityTimer = 0;
    workGravity_.isOverHeat = false;

    workInvincible_.invincibleTimer = 0;
    workInvincible_.isInvincible = false;

    hpSprite_->SetScale({ 10.0f * hp_, 64.0f });

    dashSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/dash.wav");
    deathSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/disolve.wav");
    shootSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/shoot.wav");
    crashSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/crash.wav");

    RegisterGlobalVariables();

    currentAnimation_ = PlayerModel::kWait;
    animationParameter_ = 0.0f;

}

void Player::Update() {

    ApplyGlobalVariables();

    prePosition_ = transform.worldMatrix.GetTranslate();

    isBreak_ = false;

    if (workInvincible_.isInvincible) {

        if (--workInvincible_.invincibleTimer <= 0) {
            workInvincible_.isInvincible = false;
        }

    }

    if (!isDead_) {

        if (behaviorRequest_) {
            //振る舞いを変更する
            behavior_ = behaviorRequest_.value();
            //各振る舞いごとの初期化を実行
            switch (behavior_) {
            case Behavior::kRoot:
            default:
                BehaviorRootInitialize();
                break;
            case Behavior::kAttack:
                BehaviorAttackInitialize();
                break;
            case Behavior::kShot:
                BehaviorShotInitialize();
                break;
            }
            //振る舞いリクエストをリセット
            behaviorRequest_ = std::nullopt;
        }

        switch (behavior_) {
        case Behavior::kRoot:
        default:
            BehaviorRootUpdate();
            break;
        case Behavior::kAttack:
            BehaviorAttackUpdate();
            break;
        case Behavior::kShot:
            BehaviorShotUpdate();
            break;
        }

        if (isBreak_) {

        }
        else {

        }

        if (hp_ <= 0) {
            isDead_ = true;
            Audio::GetInstance()->SoundPlayWave(deathSE_);
        }
    }

    hpSprite_->SetScale({ 10.0f * hp_, 64.0f });

    /* for (uint32_t i = 0; i < kMaxParts; i++) {
         playerTransforms_[i]->UpdateMatrix();
         playerModels_[i]->SetWorldMatrix(playerTransforms_[i]->worldMatrix);
     }*/

    reticle_->Update();

    weapon_->Update();

    collider_->SetCenter(transform.translate);
    collider_->SetOrientation(transform.rotate);
    shockWaveCollider_->SetCenter(transform.translate);

    if (!isDead_ && workInvincible_.invincibleTimer % 2 == 0) {
        model_.SetIsActive(true);
    }
    else {
        model_.SetIsActive(false);
    }

    if (!isDead_) {
        weapon_->GetModel()->SetIsActive(true);
    }
    else {
        weapon_->GetModel()->SetIsActive(false);
    }

    //状態に応じてUIの表示を変更
    if (behavior_ != Behavior::kRoot) {

        ui_LB_->SetColor({ 1.0f,1.0f,1.0f,0.5f });
        ui_RB_->SetColor({ 1.0f,1.0f,1.0f,0.5f });
        ui_RT_->SetColor({ 1.0f,1.0f,1.0f,0.5f });

        if (behavior_ == Behavior::kShot) {
            ui_A_->SetColor({ 1.0f,1.0f,1.0f,0.5f });
        }

    }
    else {

        //ダッシュ、攻撃UI
        if (workGravity_.gravityTimer <= 0 && !isPoseShot_) {
            ui_LB_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
            ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
        }
        else {
            ui_LB_->SetColor({ 1.0f,1.0f,1.0f,0.5f });
            ui_A_->SetColor({ 1.0f,1.0f,1.0f,0.5f });
        }

        //突き立てUI
        if (!isPoseShot_ && !workGravity_.isOverHeat) {
            ui_RB_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
        }
        else {
            ui_RB_->SetColor({ 1.0f,1.0f,1.0f,0.5f });
        }

        //重力発射UI
        if (workGravity_.gravityTimer <= 0 && weapon_->GetIsGravity()) {

            //重力の大きさに応じてUIの色変更
            switch (weapon_->GetLevel())
            {
            default:
            case Weapon::kSmall:
                ui_RT_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
                break;
            case Weapon::kMedium:
                ui_RT_->SetColor({ 1.0f,1.0f,0.0f,1.0f });
                break;
            case Weapon::kWide:
                ui_RT_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
                break;
            }

        }
        else {
            ui_RT_->SetColor({ 1.0f,1.0f,1.0f,0.5f });
        }

    }

    static float t = 0.0f;
    t += 1.0f / 120.0f;
    if (t > 1.0f) { t -= 1.0f; }

    transform.UpdateMatrix();
    modelScaleTransform_.UpdateMatrix();


    model_.Update(currentAnimation_, animationParameter_);
}

void Player::BehaviorRootUpdate() {

    auto& camera = camera_->GetCamera();
    auto input = Input::GetInstance();

    auto& xinputState = input->GetXInputState();

    auto& preXInputState = input->GetPreXInputState();

    //突き立て処理まとめ
    Thrust();

    //重力波発射準備
    if (xinputState.Gamepad.bRightTrigger && workGravity_.gravityTimer <= 0) {

        //重力付与状態で構える
        if (weapon_->GetIsGravity()) {
            //playerTransforms_[kRightUpperArm]->rotate = Quaternion::MakeFromAngleAxis(-2.32f, Vector3{ 1.0f,0.0f,0.0f }.Normalized());
            weapon_->modelBodyTransform_->rotate = Quaternion::MakeFromAngleAxis(1.0f, Vector3{ 1.0f,0.0f,0.0f }.Normalized()) * Quaternion::identity;
            isPoseShot_ = true;
        }

    }

    //トリガーを離した時に重力波発射
    if (preXInputState.Gamepad.bRightTrigger && !xinputState.Gamepad.bRightTrigger) {

        //重力付与状態で発射していなかったら
        if (/*weapon_->GetIsGravity() && */!weapon_->GetIsShot() && isPoseShot_) {
            weapon_->Shot(reticle_->GetReticlePosition() - weapon_->GetPosition());
            isPoseShot_ = false;
            behaviorRequest_ = Behavior::kShot;
        }

    }

    // 攻撃に遷移
    // 重力波発射中と突き出し中は遷移不可
    if (((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
        !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) &&
        !weapon_->GetIsShot() && workGravity_.gravityTimer <= 0 && !isPoseShot_) {


        attack_.attackType = kHorizontal_1;

        behaviorRequest_ = Behavior::kAttack;

    }
    // ダッシュ
    if ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) &&
        workGravity_.gravityTimer <= 0 && !isPoseShot_) {

        //ディレイの値に応じてスピード調整
        workDash_.speed_ = 2.0f - (weapon_->GetDelay() / 20.0f);

    }
    else {
        workDash_.speed_ = 1.0f - (weapon_->GetDelay() / 40.0f);
    }

    Vector3 move{};
    // Gamepad入力
    {
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
            move = move.Normalized() * (0.7f * workDash_.speed_);

            // 親がいる場合親の空間にする
            const Transform* parent = transform.GetParent();
            if (parent) {
                move = parent->worldMatrix.Inverse().ApplyRotation(move);
            }

            // 移動
            transform.translate += move;
            // 回転
            //playerTransforms_[kHip]->rotate = Quaternion::Slerp(0.2f, playerTransforms_[kHip]->rotate, Quaternion::MakeLookRotation(move));

            if (weapon_->isThrust_ || isPoseShot_) {

            }
            else {

            }

            move = transform.rotate.Conjugate() * move;
            Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
            transform.rotate = Quaternion::Slerp(0.2f, Quaternion::identity, diff) * transform.rotate;

        }
    }

    animationParameter_ += 1.0f / waitAnimationParameter_.allFrame;
    if (animationParameter_ > 1.0f) {
        animationParameter_ -= static_cast<float>(static_cast<int>(animationParameter_));
    }

    /*if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
        workAttack_.attackType = kHorizontal;
        behaviorRequest_ = Behavior::kAttack;
    }*/

}

void Player::BehaviorRootInitialize() {
    animationParameter_ = 0.0f;
    currentAnimation_ = PlayerModel::kWait;

}

void Player::Thrust() {

    auto input = Input::GetInstance();

    auto& xinputState = input->GetXInputState();

    //重力波が発射されていない時、オーバーヒートしていない時
    if (!weapon_->GetIsShot() && !workGravity_.isOverHeat) {

        //重力を付けるまでに若干のラグを設ける
        if ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
            !weapon_->GetIsShot() && !isPoseShot_) {

            if (workGravity_.gravityTimer < workGravity_.gravityFrame) {
                workGravity_.gravityTimer++;
            }

        }
        else {

            if (workGravity_.gravityTimer > 0) {
                workGravity_.gravityTimer--;

                if (workGravity_.gravityTimer <= 0) {
                    weapon_->SetDefault();
                }

            }

        }

        //重力機能がオンになった状態
        if (workGravity_.gravityTimer >= workGravity_.gravityFrame) {

            //重力が付与されていなかったら重力付与
            if (!weapon_->GetIsGravity()) {
                weapon_->AddGravity();
            }

            weapon_->modelBodyTransform_->translate = { 0.0f,0.0f,0.0f };
            weapon_->modelBodyTransform_->rotate = Quaternion::MakeFromAngleAxis(1.57f, Vector3{ 1.0f,0.0f,0.0f }.Normalized()) * Quaternion::identity;
            weapon_->isThrust_ = true;

            //規定時間以上重力を続けると一定時間強制的に使えなくなる
            if (workGravity_.overHeatTimer < workGravity_.keepTime) {

                workGravity_.overHeatTimer++;

                if (workGravity_.overHeatTimer >= workGravity_.keepTime) {
                    workGravity_.isOverHeat = true;
                    workGravity_.gravityTimer = 0;
                    weapon_->SetDefault();
                }

            }

        }
        else {

            //持続していない間は重力時間を回復
            if (workGravity_.overHeatTimer > 0) {
                workGravity_.overHeatTimer--;

            }

            weapon_->isThrust_ = false;
        }

    }
    else {
        weapon_->isThrust_ = false;
    }

    //オーバーヒート中はタイマーを回復
    if (workGravity_.isOverHeat) {

        //規定時間到達で解除
        if (workGravity_.overHeatTimer > 0) {

            workGravity_.overHeatTimer--;

            if (workGravity_.overHeatTimer <= 0) {
                workGravity_.isOverHeat = false;
            }

        }

    }

}

void Player::BehaviorAttackUpdate() {

    auto input = Input::GetInstance();

    auto& xinputState = input->GetXInputState();

    auto& preXInputState = input->GetPreXInputState();

    // コンボを進めるラムダ式
    auto IsCombo = [&]() {
        ui_A_->SetColor({ 1.0f,1.0f,0.0f,1.0f });
        if (!attack_.isCombo_ &&
            (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
            !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
            attack_.isCombo_ = true;
        }
    };


    switch (attack_.attackType)
    {
    default:
    case AttackType::kHorizontal_1:

        // アニメーションが半分を超えたらコンボ受付開始
        if (animationParameter_ >= 0.5f) {
            IsCombo();
        }

        animationParameter_ += 1.0f / workAttack_01_.allFrame;
        if (animationParameter_ > 1.0f) {
            animationParameter_ = 0.0f;
            if (attack_.isCombo_) {
                attack_.currentCombo_++;
                attack_.isCombo_ = false;
                attack_.attackType = kHorizontal_2;
                ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
                BehaviorAttackInitialize();
            }
            else {
                weapon_->SetDefault();
                transform.rotate = attack_.playerRotate;
                attack_.currentCombo_ = 0;
                attack_.isCombo_ = false;
                weapon_->GetCollider()->SetIsActive(false);
                weapon_->isAttack_ = false;
                behaviorRequest_ = Behavior::kRoot;
                ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
            }
        }

        break;
    case AttackType::kHorizontal_2:

        // アニメーションが半分を超えたらコンボ受付開始
        if (animationParameter_ >= 0.5f) {
            IsCombo();
        }

        animationParameter_ += 1.0f / workAttack_02_.allFrame;
        if (animationParameter_ > 1.0f) {
            animationParameter_ = 0.0f;
            if (attack_.isCombo_) {
                attack_.currentCombo_++;
                attack_.isCombo_ = false;
                attack_.attackType = kRotateAttack;
                ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
                BehaviorAttackInitialize();
            }
            else {
                weapon_->SetDefault();
                transform.rotate = attack_.playerRotate;
                attack_.currentCombo_ = 0;
                attack_.isCombo_ = false;
                weapon_->GetCollider()->SetIsActive(false);
                weapon_->isAttack_ = false;
                behaviorRequest_ = Behavior::kRoot;
                ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
            }
        }

        break;
    case AttackType::kRotateAttack:
        animationParameter_ += 1.0f / workAttack_03_.allFrame;
        if (animationParameter_ > 1.0f) {
            animationParameter_ = 0.0f;

            weapon_->SetDefault();
            transform.rotate = attack_.playerRotate;
            attack_.currentCombo_ = 0;
            attack_.isCombo_ = false;
            weapon_->GetCollider()->SetIsActive(false);
            weapon_->isAttack_ = false;
            behaviorRequest_ = Behavior::kRoot;
            ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
        }

        break;

    }

}

void Player::BehaviorAttackInitialize() {

    animationParameter_ = 0.0f;
    attack_.attackTimer = 0;
    attack_.isCombo_ = false;
    weapon_->isHit_ = false;
    weapon_->GetGravityCollider()->SetName("Gravity_Attack");

    switch (attack_.attackType)
    {
    default:
    case AttackType::kHorizontal_1:

        attack_.playerRotate = transform.rotate;

        weapon_->modelBodyTransform_->translate = { 0.0f,0.0f,0.0f };

        currentAnimation_ = PlayerModel::kAttack1;
        weapon_->SetIsAttack(true);

        break;
    case AttackType::kHorizontal_2:

        currentAnimation_ = PlayerModel::kAttack2;

        break;
    case AttackType::kRotateAttack:

        workAttack_03_.velocity = { 0.0f,0.0f,1.0f };

        workAttack_03_.velocity = workAttack_03_.velocity = attack_.playerRotate * workAttack_03_.velocity;

        currentAnimation_ = PlayerModel::kAttack3;

        break;

    }



}

void Player::BehaviorShotUpdate() {

    if (workShot_.shotTimer < 15) {

        //武器を持っている腕を動かす
      //  playerTransforms_[kRightUpperArm]->rotate = Quaternion::Slerp(1.0f / 15.0f, Quaternion::identity,
      //      Quaternion::MakeFromAngleAxis(workShot_.shotRotate, Vector3{ 1.0f,0.0f,0.0f }.Normalized())) * playerTransforms_[kRightUpperArm]->rotate;

    }
    //else if (workShot_.shotTimer > 45) {

    //	//武器を持っている腕を動かす
    //	playerTransforms_[kRightLowerArm]->rotate = Quaternion::Slerp(1.0f / 15.0f, Quaternion::identity,
    //		Quaternion::MakeFromAngleAxis(workShot_.shotRotate, Vector3{ 1.0f,0.5f,0.5f }.Normalized())) * playerTransforms_[kRightLowerArm]->rotate;

    //}

    if (++workShot_.shotTimer >= workShot_.maxShotFrame) {

        // playerTransforms_[kRightUpperArm]->rotate = Quaternion::identity;

        workShot_.shotTimer = 0;

        behaviorRequest_ = Behavior::kRoot;

    }

}

void Player::BehaviorShotInitialize() {

    workShot_.shotTimer = 0;

}

void Player::Damage(uint32_t val) {

    if (!workInvincible_.isInvincible) {
        hp_ -= val;
        workInvincible_.invincibleTimer = 60;
        workInvincible_.isInvincible = true;
    }

}

void Player::OnCollision(const CollisionInfo& collisionInfo) {

    if (collisionInfo.collider->GetName() == "Enemy_Bullet" ||
        collisionInfo.collider->GetName() == "Small_Enemy" ||
        collisionInfo.collider->GetName() == "Barrier_Enemy") {

        Damage(1);

    }

}

constexpr char kGroupName[] = "Player";

void Player::RegisterGlobalVariables() {

    GlobalVariables& globalVariables = *GlobalVariables::GetInstance();

    if (!globalVariables.HasGroup(kGroupName)) {
        auto& group = globalVariables[kGroupName];
        group["Dush Speed"] = workDash_.speed_;
       /* group["Attack PreFrame"] = workAttack_01_.preFrame;
        group["Attack WaitFrameBefore"] = workAttack_01_.waitFrameBefore;
        group["Attack WaitFrameAfter"] = workAttack_01_.waitFrameAfter;
        group["Attack AttackFrame"] = workAttack_01_.attackFrame;
        group["Attack PreRotate"] = workAttack_01_.preRotate;
        group["Attack AttackRotate"] = workAttack_01_.attackRotate;*/
    }

}

void Player::ApplyGlobalVariables() {

    //攻撃中は変数を変更しない
    if (behavior_ != Behavior::kAttack) {

        GlobalVariables& globalVariables = *GlobalVariables::GetInstance();

        auto& group = globalVariables[kGroupName];

        workDash_.speed_ = group["Dush Speed"].Get<float>();
        //workAttack_01_.preFrame = group["Attack PreFrame"].Get<int32_t>() + weapon_->GetDelay();
        //workAttack_01_.waitFrameBefore = group["Attack WaitFrameBefore"].Get<int32_t>() + weapon_->GetDelay();
        //workAttack_01_.waitFrameAfter = group["Attack WaitFrameAfter"].Get<int32_t>() + weapon_->GetDelay();
        //workAttack_01_.attackFrame = group["Attack AttackFrame"].Get<int32_t>();
        //workAttack_01_.preRotate = group["Attack PreRotate"].Get<float>();
        //workAttack_01_.attackRotate = group["Attack AttackRotate"].Get<float>();

        ////攻撃に使う合計フレームを設定
        //workAttack_01_.allFrame = workAttack_01_.preFrame + workAttack_01_.waitFrameBefore +
        //    workAttack_01_.attackFrame + workAttack_01_.waitFrameAfter;

    }

}


