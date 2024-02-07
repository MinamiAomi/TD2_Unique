#include "Player.h"
#include "Engine/input/Input.h"
#include "Graphics/ResourceManager.h"
#include "Game/block/block.h"
#include "GlobalVariables.h"
#include "Game/Enemy/BulletManager.h"
#include "Game/enemy/BarrierBulletManager.h"
#include "Game/enemy/SmallEnemyManager.h"
#include "Graphics/ImGuiManager.h"

Player::Player() {
    /*auto resources = ResourceManager::GetInstance();*/

    hpTex_ = ResourceManager::GetInstance()->FindTexture("player_hp");
    hpOverTex_ = ResourceManager::GetInstance()->FindTexture("player_hp_over");
    hpUnderTex_ = ResourceManager::GetInstance()->FindTexture("player_hp_under");
    guardTex_ = ResourceManager::GetInstance()->FindTexture("guard_gauge");

    hpSprite_ = std::make_unique<Sprite>();
    hpSprite_->SetTexture(hpTex_);
    hpSprite_->SetPosition({ 156.0f - 104.0f,78.0f - 27.0f });
    hpSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 514.0f,322.0f });
    hpSprite_->SetDrawOrder(2);
	hpOverSprite_ = std::make_unique<Sprite>();
	hpOverSprite_->SetTexture(hpOverTex_);
	hpOverSprite_->SetPosition({ 156.0f,78.0f });
	hpOverSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 514.0f,322.0f });
	hpOverSprite_->SetDrawOrder(3);

	hpUnderSprite_ = std::make_unique<Sprite>();
	hpUnderSprite_->SetTexture(hpUnderTex_);
	hpUnderSprite_->SetPosition({ 156.0f,78.0f });
	hpUnderSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 514.0f,322.0f });
	hpUnderSprite_->SetDrawOrder(1);

	guardSprite_ = std::make_unique<Sprite>();
	guardSprite_->SetTexture(guardTex_);
	guardSprite_->SetPosition({ 938.0f,50.0f });

	ui_A_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_A");
	ui_RB_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_RB");
	ui_LB_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_LB");
	ui_RT_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_RT");
	ui_RS_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_RS");
	ui_LS_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_LS");

	ui_A_ = std::make_unique<Sprite>();
	ui_A_->SetTexture(ui_A_Tex_);
	ui_A_->SetPosition({ 1180.0f,150.0f });
	ui_A_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
	ui_A_->SetScale({ 192.0f,48.0f });

	ui_RB_ = std::make_unique<Sprite>();
	ui_RB_->SetTexture(ui_RB_Tex_);
	ui_RB_->SetPosition({ 1130.0f,50.0f });
	ui_RB_->SetTexcoordRect({ 512.0f,64.0f }, { 512.0f,64.0f });
	ui_RB_->SetScale({ 384.0f,48.0f });

	ui_LB_ = std::make_unique<Sprite>();
	ui_LB_->SetTexture(ui_LB_Tex_);
	ui_LB_->SetPosition({ 1170.0f,200.0f });
	ui_LB_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
	ui_LB_->SetScale({ 192.0f,48.0f });

	ui_RT_ = std::make_unique<Sprite>();
	ui_RT_->SetTexture(ui_RT_Tex_);
	ui_RT_->SetPosition({ 1180.0f,100.0f });
	ui_RT_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
	ui_RT_->SetScale({ 192.0f,48.0f });

	ui_RS_ = std::make_unique<Sprite>();
	ui_RS_->SetTexture(ui_RS_Tex_);
	ui_RS_->SetPosition({ 1100.0f,250.0f });
	ui_RS_->SetTexcoordRect({ 512.0f,64.0f }, { 512.0f,64.0f });
	ui_RS_->SetScale({ 384.0f,48.0f });

	ui_LS_ = std::make_unique<Sprite>();
	ui_LS_->SetTexture(ui_LS_Tex_);
	ui_LS_->SetPosition({ 1100.0f,300.0f });
	ui_LS_->SetTexcoordRect({ 512.0f,64.0f }, { 512.0f,64.0f });
	ui_LS_->SetScale({ 384.0f,48.0f });

    collider_ = std::make_unique<BoxCollider>();

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
    transform.translate = { 0.0f, 8.5f, -30.0f };
    modelScaleTransform_.SetParent(&transform);
    modelScaleTransform_.scale = { 2.0f, 2.0f, 2.0f };
    modelScaleTransform_.rotate = Quaternion::identity;
    modelScaleTransform_.translate = Vector3::zero;
    model_.Initialize(&modelScaleTransform_);

    weapon_->modelBodyTransform_->SetParent(&model_.GetTransform(PlayerModel::kRightLowerArm));
    weapon_->Initialize();
    weapon_->SetPlayer(this);
    weapon_->SetDefault();
    weapon_->Update();

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

	velocity_ = { 0.0f,0.0f,1.0f };
	knockBackVelocity_ = Vector3::zero;
	isKnockBack_ = false;

	diff_ = Quaternion::identity;

	behavior_ = Behavior::kRoot;

    workDash_.speed_ = 2.0f;

    attack_.attackType = kVertical;
    attack_.isCombo_ = false;

    workGravity_.gravityTimer = 0;
    workGravity_.isOverHeat = false;

    workInvincible_.invincibleTimer = 0;
    workInvincible_.isInvincible = false;

	hpSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 38.4f * hp_, 128.0f });
	hpSprite_->SetScale({ 19.2f * hp_, 64.0f });
	hpSprite_->SetAnchor({ 0.0f,0.5f });
	guardSprite_->SetScale({ 1.0f * (120 - workGravity_.overHeatTimer), 64.0f });

	hpOverSprite_->SetScale({ 257.0f, 161.0f });

	hpUnderSprite_->SetScale({ 257.0f, 161.0f });
	
	guardSprite_->SetScale({ 3.2f * (120 - workGravity_.overHeatTimer), 48.0f });
	guardSprite_->SetAnchor({ 0.0f,0.5f });

	dashSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/dash.wav");
	deathSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/disolve.wav");
	shootSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/shoot.wav");
	crashSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/crash.wav");
    attackSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/sound/hammerAttack.wav");

    RegisterGlobalVariables();

    currentAnimation_ = PlayerModel::kWait;
    animationParameter_ = 0.0f;

}


void Player::Update() {

//#ifdef _DEBUG
//
//    ImGui::Begin("pos");
//    ImGui::DragFloat3("pos", &thrustPos_.x, 0.05f);
//    ImGui::End();
//
//#endif // _DEBUG

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

        if (isStart_ && hp_ <= 0) {
            isDead_ = true;
            Audio::GetInstance()->SoundPlayWave(deathSE_);
        }
    }

	hpSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 38.4f * hp_, 128.0f });
	hpSprite_->SetScale({ 19.2f * hp_, 64.0f });
	guardSprite_->SetScale({ 2.8f * (120 - workGravity_.overHeatTimer), 48.0f });

	if (transform.translate.x > 200.0f) {
        transform.translate.x = 200.0f;
	}

	if (transform.translate.x < -200.0f) {
        transform.translate.x = -200.0f;
	}

	if (transform.translate.z > 200.0f) {
        transform.translate.z = 200.0f;
	}

	if (transform.translate.z < -200.0f) {
        transform.translate.z = -200.0f;
	}

	/*for (uint32_t i = 0; i < kMaxParts; i++) {
		playerTransforms_[i]->UpdateMatrix();
		playerModels_[i]->SetWorldMatrix(playerTransforms_[i]->worldMatrix);
	}*/

    reticle_->Update();

    weapon_->Update();

    collider_->SetCenter(transform.translate);
    collider_->SetOrientation(transform.rotate);

    if (!isDead_ && (workInvincible_.invincibleTimer % 2 == 0 || knockBackVelocity_.Length() > 0.05f)) {
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
	if (!isStart_) {
		ui_A_->SetIsActive(false);
		ui_RB_->SetIsActive(false);
		ui_LB_->SetIsActive(false);
		ui_LS_->SetIsActive(false);
		ui_RS_->SetIsActive(false);
		ui_RT_->SetIsActive(false);
		hpOverSprite_->SetIsActive(false);
		hpUnderSprite_->SetIsActive(false);
		hpSprite_->SetIsActive(false);
		guardSprite_->SetIsActive(false);
		reticle_->SetIsActive(false);
	}
	else {
		ui_A_->SetIsActive(true);
		ui_RB_->SetIsActive(true);
		ui_LB_->SetIsActive(true);
		ui_LS_->SetIsActive(true);
		ui_RS_->SetIsActive(true);
		ui_RT_->SetIsActive(true);
		hpOverSprite_->SetIsActive(true);
		hpUnderSprite_->SetIsActive(true);
		hpSprite_->SetIsActive(true);
		guardSprite_->SetIsActive(true);
		reticle_->SetIsActive(true);
	}

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

	//ゲーム開始していない時は動かせないようにする
	/*if (!isStart_) {
		return;
	}*/

	auto& camera = camera_->GetCamera();
	auto input = Input::GetInstance();

    auto& xinputState = input->GetXInputState();

    auto& preXInputState = input->GetPreXInputState();

    //突き立て処理まとめ
    Thrust();

    //重力波発射準備
    if (isStart_ && xinputState.Gamepad.bRightTrigger && workGravity_.gravityTimer <= 0) {

        //重力付与状態で構える
        if (weapon_->GetIsGravity()) {
            //playerTransforms_[kRightUpperArm]->rotate = Quaternion::MakeFromAngleAxis(-2.32f, Vector3{ 1.0f,0.0f,0.0f }.Normalized());
            weapon_->modelBodyTransform_->rotate = Quaternion::MakeForXAxis(Math::ToRadian * -120.0f);
            isPoseShot_ = true;
        }

    }

    //トリガーを離した時に重力波発射
    if (isStart_ && preXInputState.Gamepad.bRightTrigger && !xinputState.Gamepad.bRightTrigger) {

        //重力付与状態で発射していなかったら
        if (/*weapon_->GetIsGravity() && */!weapon_->GetIsShot() && isPoseShot_) {
            weapon_->Shot(reticle_->GetReticlePosition() - weapon_->GetPosition());
            isPoseShot_ = false;
            behaviorRequest_ = Behavior::kShot;
        }

    }

    // 攻撃に遷移
    // 重力波発射中と突き出し中は遷移不可
    if (isStart_ && ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
        !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) &&
        !weapon_->GetIsShot() && workGravity_.gravityTimer <= 0 && !isPoseShot_) {


        attack_.attackType = kHorizontal_1;

        behaviorRequest_ = Behavior::kAttack;

	}
	// ダッシュ
	if (isStart_ && (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) &&
		workGravity_.gravityTimer <= 0 && !isPoseShot_) {
		
		//ディレイの値に応じてスピード調整
		workDash_.speed_ = 2.0f - (weapon_->gravityDelay_ / 50.0f) - workGravity_.decel;

	}
	else {
		workDash_.speed_ = 1.0f - (weapon_->gravityDelay_ / 70.0f) - workGravity_.decel;
	}

	Vector3 move{};
	// Gamepad入力(ノックバックしていない時)
	if (isStart_ && !isKnockBack_) {

		const float margin = 0.8f;
		const float shortMaxReci = 1.0f / float(SHRT_MAX);
		move = { float(xinputState.Gamepad.sThumbLX), 0.0f, float(xinputState.Gamepad.sThumbLY) };
		move *= shortMaxReci;
		if (move.Length() < margin) {
			move = Vector3::zero;
		}

	}
    //ノックバックしている時
	else if(isKnockBack_) {

		move = knockBackVelocity_;
		
		//徐々に速度を減らす
		knockBackVelocity_ /= 1.06f;

		//既定の速度以下でノックバック終了
		if (knockBackVelocity_.Length() < 0.1f) {
			knockBackVelocity_ = Vector3::zero;
			isKnockBack_ = false;
		}

	}

	// 移動、回転処理
	{
		if (move != Vector3::zero) {
			

			if (!isKnockBack_) {
				move = move.Normalized();
				// 地面に水平なカメラの回転
				move = camera->GetRotate() * move;
				move.y = 0.0f;
				move = move.Normalized() * (0.7f * workDash_.speed_);
			}
			/*else {
				
			}*/

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

			if (!isKnockBack_) {
				move = transform.rotate.Conjugate() * move;

                if (fabsf(Vector3::Dot(Vector3::unitZ, move)) != 0.0f) {
                    Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
                    transform.rotate = Quaternion::Slerp(0.7f, Quaternion::identity, diff) * transform.rotate;
                }

			}
            

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

            weapon_->modelBodyTransform_->translate = thrustPos_;
            weapon_->modelBodyTransform_->rotate = Quaternion::MakeForXAxis(Math::ToRadian * -100.0f);
			weapon_->isThrust_ = true;
			workGravity_.decel = workGravity_.decelVal;

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
			workGravity_.decel = 0.0f;

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

    //ノックバックしたら強制終了
    if (isKnockBack_) {
        weapon_->SetDefault();
        transform.rotate = attack_.playerRotate;
        attack_.currentCombo_ = 0;
        attack_.isCombo_ = false;
        weapon_->isShockWave_ = false;
        weapon_->shockWaveCollider_->SetIsActive(false);
        weapon_->GetCollider()->SetIsActive(false);
        weapon_->isAttack_ = false;
        behaviorRequest_ = Behavior::kRoot;
        ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    }

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
                weapon_->isShockWave_ = false;
                weapon_->shockWaveCollider_->SetIsActive(false);
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
                weapon_->isShockWave_ = false;
                weapon_->shockWaveCollider_->SetIsActive(false);
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
            weapon_->isShockWave_ = false;
            weapon_->shockWaveCollider_->SetIsActive(false);
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

    Audio::GetInstance()->SoundPlayWave(attackSE_);

}

void Player::BehaviorShotUpdate() {

    if (workShot_.shotTimer < 15) {

    }
    //else if (workShot_.shotTimer > 45) {

    //	//武器を持っている腕を動かす
    //	playerTransforms_[kRightLowerArm]->rotate = Quaternion::Slerp(1.0f / 15.0f, Quaternion::identity,
    //		Quaternion::MakeFromAngleAxis(workShot_.shotRotate, Vector3{ 1.0f,0.5f,0.5f }.Normalized())) * playerTransforms_[kRightLowerArm]->rotate;

    //}

    if (++workShot_.shotTimer >= workShot_.maxShotFrame) {

        workShot_.shotTimer = 0;

        behaviorRequest_ = Behavior::kRoot;

    }

}

void Player::BehaviorShotInitialize() {

    workShot_.shotTimer = 0;

}

void Player::KnockBack(const Vector3& affectPosition) {

	knockBackVelocity_ = transform.worldMatrix.GetTranslate() - affectPosition;
	knockBackVelocity_.y = 0.0f;
	knockBackVelocity_ = knockBackVelocity_.Normalized();

	isKnockBack_ = true;

}

void Player::Damage(uint32_t val, const Vector3& affectPosition) {

	if (!workInvincible_.isInvincible) {
		hp_ -= val;
		workInvincible_.invincibleTimer = 120;
		workInvincible_.isInvincible = true;
		KnockBack(affectPosition);
	}

}

void Player::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Small_Enemy") {

		auto object = collisionInfo.collider->GetGameObject();

		auto enemy = SmallEnemyManager::GetInstance()->GetEnemy(object);

		Damage(1, enemy->transform.worldMatrix.GetTranslate());

	}
	else if (collisionInfo.collider->GetName() == "Barrier_Enemy") {

		auto object = collisionInfo.collider->GetGameObject();

		auto enemy = SmallEnemyManager::GetInstance()->GetEnemy(object);

		Damage(1, enemy->transform.worldMatrix.GetTranslate());

	}
	else if (collisionInfo.collider->GetName() == "Enemy_Bullet") {

		auto object = collisionInfo.collider->GetGameObject();

		auto bullet = BulletManager::GetInstance()->GetBullet(object);

		Damage(1, bullet->transform.worldMatrix.GetTranslate());

        bullet->SetIsDead(true);

	}
	else if (collisionInfo.collider->GetName() == "Barrier_Bullet") {

		auto object = collisionInfo.collider->GetGameObject();

		auto bullet = BarrierBulletManager::GetInstance()->GetBullet(object);

		Damage(1, bullet->transform.worldMatrix.GetTranslate());

        bullet->SetIsDead(true);

    }

}

constexpr char kGroupName[] = "Player";

void Player::RegisterGlobalVariables() {

    GlobalVariables& globalVariables = *GlobalVariables::GetInstance();

    if (!globalVariables.HasGroup(kGroupName)) {
        auto& group = globalVariables[kGroupName];
        group["Dush Speed"] = workDash_.speed_;
        group["Attack_01 Frame"] = workAttack_01_.allFrame;
        group["Attack_02 Frame"] = workAttack_02_.allFrame;
        group["Attack_03 Frame"] = workAttack_03_.allFrame;
    }

}

void Player::ApplyGlobalVariables() {

    //攻撃中は変数を変更しない
    if (behavior_ != Behavior::kAttack) {

        GlobalVariables& globalVariables = *GlobalVariables::GetInstance();

        auto& group = globalVariables[kGroupName];

        workDash_.speed_ = group["Dush Speed"].Get<float>();
        workAttack_01_.allFrame = group["Attack_01 Frame"].Get<int32_t>();
        workAttack_02_.allFrame = group["Attack_02 Frame"].Get<int32_t>();
        workAttack_03_.allFrame = group["Attack_03 Frame"].Get<int32_t>();

        workAttack_01_.allFrame = workAttack_01_.allFrame + weapon_->gravityDelay_;
        workAttack_02_.allFrame = workAttack_02_.allFrame + weapon_->gravityDelay_;
        workAttack_03_.allFrame = workAttack_03_.allFrame + weapon_->gravityDelay_;

    }

}


