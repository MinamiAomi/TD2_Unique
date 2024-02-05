#include "Player.h"
#include "Engine/input/Input.h"
#include "Graphics/ResourceManager.h"
#include "Game/block/block.h"
#include "GlobalVariables.h"
#include "Game/Enemy/BulletManager.h"
#include "Game/enemy/BarrierBulletManager.h"
#include "Game/enemy/SmallEnemyManager.h"

Player::Player()
{

	playerModel_ = std::make_shared<ModelInstance>();
	playerModel_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));

	for (uint32_t i = 0; i < kMaxParts; i++) {
		playerModels_[i] = std::make_shared<ModelInstance>();
		playerTransforms_[i] = std::make_shared<Transform>();
	}

	playerModels_[kHip]->SetModel(ResourceManager::GetInstance()->FindModel("Hip"));
	playerModels_[kBody]->SetModel(ResourceManager::GetInstance()->FindModel("Body"));
	playerModels_[kHead]->SetModel(ResourceManager::GetInstance()->FindModel("Head"));
	playerModels_[kLeftShoulder]->SetModel(ResourceManager::GetInstance()->FindModel("LeftShoulder"));
	playerModels_[kLeftUpperArm]->SetModel(ResourceManager::GetInstance()->FindModel("LeftUpperArm"));
	playerModels_[kLeftLowerArm]->SetModel(ResourceManager::GetInstance()->FindModel("LeftLowerArm"));
	playerModels_[kRightShoulder]->SetModel(ResourceManager::GetInstance()->FindModel("RightShoulder"));
	playerModels_[kRightUpperArm]->SetModel(ResourceManager::GetInstance()->FindModel("RightUpperArm"));
	playerModels_[kRightLowerArm]->SetModel(ResourceManager::GetInstance()->FindModel("RightLowerArm"));
	playerModels_[kLeftUpperLeg]->SetModel(ResourceManager::GetInstance()->FindModel("LeftUpperLeg"));
	playerModels_[kLeftLowerLeg]->SetModel(ResourceManager::GetInstance()->FindModel("LeftLowerLeg"));
	playerModels_[kRightUpperLeg]->SetModel(ResourceManager::GetInstance()->FindModel("RightUpperLeg"));
	playerModels_[kRightLowerLeg]->SetModel(ResourceManager::GetInstance()->FindModel("RightLowerLeg"));

	hpTex_ = ResourceManager::GetInstance()->FindTexture("player_hp");
	hpOverTex_ = ResourceManager::GetInstance()->FindTexture("player_hp_over");
	hpUnderTex_ = ResourceManager::GetInstance()->FindTexture("player_hp_under");
	guardTex_ = ResourceManager::GetInstance()->FindTexture("guard_gauge");

	hpSprite_ = std::make_unique<Sprite>();
	hpSprite_->SetTexture(hpTex_);
	hpSprite_->SetPosition({ 256.0f - 104.0f,128.0f - 27.0f });
	hpSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 514.0f,322.0f });
	hpSprite_->SetDrawOrder(2);

	hpOverSprite_ = std::make_unique<Sprite>();
	hpOverSprite_->SetTexture(hpOverTex_);
	hpOverSprite_->SetPosition({ 256.0f,128.0f });
	hpOverSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 514.0f,322.0f });
	hpOverSprite_->SetDrawOrder(3);

	hpUnderSprite_ = std::make_unique<Sprite>();
	hpUnderSprite_->SetTexture(hpUnderTex_);
	hpUnderSprite_->SetPosition({ 256.0f,128.0f });
	hpUnderSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 514.0f,322.0f });
	hpUnderSprite_->SetDrawOrder(1);

	guardSprite_ = std::make_unique<Sprite>();
	guardSprite_->SetTexture(guardTex_);
	guardSprite_->SetPosition({ 320.0f,10.0f });

	ui_A_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_A");
	ui_RB_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_RB");
	ui_LB_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_LB");
	ui_RT_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_RT");
	ui_RS_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_RS");
	ui_LS_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_LS");

	ui_A_ = std::make_unique<Sprite>();
	ui_A_->SetTexture(ui_A_Tex_);
	ui_A_->SetPosition({ 1100.0f,150.0f });
	ui_A_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
	ui_A_->SetScale({ 192.0f,48.0f });

	ui_RB_ = std::make_unique<Sprite>();
	ui_RB_->SetTexture(ui_RB_Tex_);
	ui_RB_->SetPosition({ 1050.0f,50.0f });
	ui_RB_->SetTexcoordRect({ 512.0f,64.0f }, { 512.0f,64.0f });
	ui_RB_->SetScale({ 384.0f,48.0f });

	ui_LB_ = std::make_unique<Sprite>();
	ui_LB_->SetTexture(ui_LB_Tex_);
	ui_LB_->SetPosition({ 1100.0f,200.0f });
	ui_LB_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
	ui_LB_->SetScale({ 192.0f,48.0f });

	ui_RT_ = std::make_unique<Sprite>();
	ui_RT_->SetTexture(ui_RT_Tex_);
	ui_RT_->SetPosition({ 1100.0f,100.0f });
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
	
	for (uint32_t i = 0; i < kMaxParts; i++) {
		playerTransforms_[i]->translate = Vector3::zero;
		playerTransforms_[i]->scale = Vector3::one;
		playerTransforms_[i]->rotate = Quaternion::identity;
	}

	//腰から体に親子付け
	playerTransforms_[kBody]->SetParent(playerTransforms_[kHip].get());
	//体から頭に親子付け
	playerTransforms_[kHead]->SetParent(playerTransforms_[kBody].get());
	//体から両腕に順番に親子付け
	playerTransforms_[kLeftShoulder]->SetParent(playerTransforms_[kBody].get());
	playerTransforms_[kLeftUpperArm]->SetParent(playerTransforms_[kLeftShoulder].get());
	playerTransforms_[kLeftLowerArm]->SetParent(playerTransforms_[kLeftUpperArm].get());
	playerTransforms_[kRightShoulder]->SetParent(playerTransforms_[kBody].get());
	playerTransforms_[kRightUpperArm]->SetParent(playerTransforms_[kRightShoulder].get());
	playerTransforms_[kRightLowerArm]->SetParent(playerTransforms_[kRightUpperArm].get());
	//腰から両足に順番に親子付け
	playerTransforms_[kLeftUpperLeg]->SetParent(playerTransforms_[kHip].get());
	playerTransforms_[kLeftLowerLeg]->SetParent(playerTransforms_[kLeftUpperLeg].get());
	playerTransforms_[kRightUpperLeg]->SetParent(playerTransforms_[kHip].get());
	playerTransforms_[kRightLowerLeg]->SetParent(playerTransforms_[kRightUpperLeg].get());

	//座標設定
	playerTransforms_[kHip]->translate = { 0.0f,8.0f,0.0f };
	playerTransforms_[kBody]->translate = { 0.0f,2.0f,0.0f };
	playerTransforms_[kHead]->translate = { 0.0f,2.0f,0.0f };
	playerTransforms_[kLeftShoulder]->translate = { -3.0f,1.5f,0.0f };
	playerTransforms_[kLeftUpperArm]->translate = { 0.0f,-2.0f,0.0f };
	playerTransforms_[kLeftLowerArm]->translate = { 0.0f,-3.0f,0.0f };
	playerTransforms_[kRightShoulder]->translate = { 3.0f,1.5f,0.0f };
	playerTransforms_[kRightUpperArm]->translate = { 0.0f,-2.0f,0.0f };
	playerTransforms_[kRightLowerArm]->translate = { 0.0f,-3.0f,0.0f };
	playerTransforms_[kLeftUpperLeg]->translate = { -1.0f,-2.0f,0.0f };
	playerTransforms_[kLeftLowerLeg]->translate = { 0.0f,-2.0f,0.0f };
	playerTransforms_[kRightUpperLeg]->translate = { 1.0f,-2.0f,0.0f };
	playerTransforms_[kRightLowerLeg]->translate = { 0.0f,-2.0f,0.0f };

	//回転初期化
	for (uint32_t i = 0; i < kMaxParts; i++) {
		playerTransforms_[i]->rotate = Quaternion::identity;
	}

	weapon_->modelBodyTransform_->SetParent(playerTransforms_[kRightLowerArm].get());
	weapon_->Initialize();
	weapon_->SetPlayer(this);

	reticle_->Initialize();

	preDirection_ = { 0.0f,0.0f,1.0f };
	direction_ = { 0.0f,0.0f,1.0f };

	isDead_ = false;
	hp_ = kMaxHp_;

	collider_->SetCenter(playerTransforms_[kHip]->translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(playerTransforms_[kHip]->scale * 2.0f);
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
	
	guardSprite_->SetScale({ 1.0f * (120 - workGravity_.overHeatTimer), 64.0f });
	guardSprite_->SetAnchor({ 0.0f,0.5f });

	dashSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/dash.wav");
	deathSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/disolve.wav");
	shootSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/shoot.wav");
	crashSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/crash.wav");

	RegisterGlobalVariables();

	for (uint32_t i = 0; i < kMaxParts; i++) {
		playerTransforms_[i]->UpdateMatrix();
		playerModels_[i]->SetWorldMatrix(playerTransforms_[i]->worldMatrix);
	}

	weapon_->Update();

}

void Player::Update() {

	ApplyGlobalVariables();

	prePosition_ = Vector3{
			playerTransforms_[kHip]->worldMatrix.m[3][0],
			playerTransforms_[kHip]->worldMatrix.m[3][1],
			playerTransforms_[kHip]->worldMatrix.m[3][2] };

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

	hpSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 38.4f * hp_, 128.0f });
	hpSprite_->SetScale({ 19.2f * hp_, 64.0f });
	guardSprite_->SetScale({ 1.0f * (120 - workGravity_.overHeatTimer), 64.0f });

	for (uint32_t i = 0; i < kMaxParts; i++) {
		playerTransforms_[i]->UpdateMatrix();
		playerModels_[i]->SetWorldMatrix(playerTransforms_[i]->worldMatrix);
	}

	reticle_->Update();

	weapon_->Update();

	collider_->SetCenter(playerTransforms_[kHip]->translate);
	collider_->SetOrientation(playerTransforms_[kHip]->rotate);
	playerModel_->SetWorldMatrix(playerTransforms_[kHip]->worldMatrix);

	if (!isDead_ && (workInvincible_.invincibleTimer % 2 == 0 || knockBackVelocity_.Length() > 0.05f)) {

		for (uint32_t i = 0; i < kMaxParts; i++) {
			playerModels_[i]->SetIsActive(true);
		}

		playerModel_->SetIsActive(false);
	}
	else {

		for (uint32_t i = 0; i < kMaxParts; i++) {
			playerModels_[i]->SetIsActive(false);
		}

		playerModel_->SetIsActive(false);
	}

	if (!isDead_ ) {
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

}

void Player::BehaviorRootUpdate() {

	//ゲーム開始していない時は動かせないようにする
	if (!isStart_) {
		return;
	}

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
			playerTransforms_[kRightUpperArm]->rotate = Quaternion::MakeFromAngleAxis(-2.32f, Vector3{ 1.0f,0.0f,0.0f }.Normalized());
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
		workDash_.speed_ = 2.0f - (weapon_->GetDelay() / 20.0f) - workGravity_.decel;

	}
	else {
		workDash_.speed_ = 1.0f - (weapon_->GetDelay() / 40.0f) - workGravity_.decel;
	}

	Vector3 move{};
	// Gamepad入力(ノックバックしていない時)
	if (!isKnockBack_) {

		const float margin = 0.8f;
		const float shortMaxReci = 1.0f / float(SHRT_MAX);
		move = { float(xinputState.Gamepad.sThumbLX), 0.0f, float(xinputState.Gamepad.sThumbLY) };
		move *= shortMaxReci;
		if (move.Length() < margin) {
			move = Vector3::zero;
		}

	}
	else {

		move = knockBackVelocity_;
		
		//徐々に速度を減らす
		knockBackVelocity_ /= 1.10f;

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
			const Transform* parent = playerTransforms_[kHip]->GetParent();
			if (parent) {
				move = parent->worldMatrix.Inverse().ApplyRotation(move);
			}

			// 移動
			playerTransforms_[kHip]->translate += move;
			// 回転
			//playerTransforms_[kHip]->rotate = Quaternion::Slerp(0.2f, playerTransforms_[kHip]->rotate, Quaternion::MakeLookRotation(move));

			if (weapon_->isThrust_ || isPoseShot_) {

			}
			else {

			}

			if (!isKnockBack_) {
				move = playerTransforms_[kHip]->rotate.Conjugate() * move;
				diff_ = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
				playerTransforms_[kHip]->rotate = Quaternion::Slerp(0.7f, Quaternion::identity, diff_) * playerTransforms_[kHip]->rotate;
			}
			else {

				//移動方向を反転させる
				move = move.Normalized();
				move *= -1.0f;
				move = playerTransforms_[kHip]->rotate.Conjugate() * move;
				diff_ = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
				playerTransforms_[kHip]->rotate = Quaternion::Slerp(0.7f, Quaternion::identity, diff_) * playerTransforms_[kHip]->rotate;

			}

			
			
		}
	}

	/*if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		workAttack_.attackType = kHorizontal;
		behaviorRequest_ = Behavior::kAttack;
	}*/

}

void Player::BehaviorRootInitialize() {
	
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
		/*weapon_->modelBodyTransform_.rotate = Quaternion::MakeFromTwoVector(Vector3::unitZ, Vector3{ 0.5f,0.5f,0.5f }) *
			Quaternion::identity;*/
		playerTransforms_[kHip]->rotate = attack_.playerRotate;
		attack_.currentCombo_ = 0;
		attack_.isCombo_ = false;
		weapon_->isShockWave_ = false;
		weapon_->shockWaveCollider_->SetIsActive(false);
		weapon_->GetCollider()->SetIsActive(false);
		weapon_->isAttack_ = false;
		behaviorRequest_ = Behavior::kRoot;
		ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}

	switch (attack_.attackType)
	{
	default:
	case AttackType::kHorizontal_1:

		//一定時間すぎた後、条件が揃っている状態で入力したら次のコンボ用意
		if (attack_.attackTimer > 0) {

			/*if (weapon_->GetIsGravity()) {
				ui_A_->SetColor({ 1.0f,1.0f,0.0f,1.0f });
			}*/

			ui_A_->SetColor({ 1.0f,1.0f,0.0f,1.0f });

			if (/*weapon_->GetIsGravity() && */!attack_.isCombo_ &&
				attack_.currentCombo_ < 2 &&
				(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
				!(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
				attack_.isCombo_ = true;
			}

		}

		//攻撃中
		if (attack_.attackTimer >= WA_01_.waitFrameBefore + WA_01_.preFrame &&
			attack_.attackTimer - WA_01_.waitFrameBefore - WA_01_.preFrame < WA_01_.attackFrame) {

			//重力付与時は範囲拡大
			if (weapon_->GetIsGravity()) {
				playerTransforms_[kHip]->rotate = Quaternion::Slerp(float(1.5f / WA_01_.attackFrame),
					Quaternion::identity, Quaternion::MakeForYAxis(WA_01_.attackRotate)) * playerTransforms_[kHip]->rotate;
			}
			else {
				playerTransforms_[kHip]->rotate = Quaternion::Slerp(float(1.0f / WA_01_.attackFrame),
					Quaternion::identity, Quaternion::MakeForYAxis(WA_01_.attackRotate)) * playerTransforms_[kHip]->rotate;
			}

			weapon_->GetCollider()->SetIsActive(true);
			weapon_->isAttack_ = true;
		}
		//攻撃開始前
		else if (attack_.attackTimer < WA_01_.preFrame) {
			playerTransforms_[kHip]->rotate = Quaternion::Slerp(1.0f / float(WA_01_.preFrame),
				Quaternion::identity, Quaternion::MakeForYAxis(WA_01_.preRotate)) * playerTransforms_[kHip]->rotate;
			weapon_->GetCollider()->SetIsActive(false);
			weapon_->isAttack_ = false;
		}
		else {
			weapon_->GetCollider()->SetIsActive(false);
			weapon_->isAttack_ = false;
		}
		//振り終わりの攻撃硬直中に次の攻撃の方向を決める
		//else if (workAttack_.attackTimer - workAttack_.waitFrameBefore - workAttack_.preFrame >= workAttack_.attackFrame) {

		//	Vector3 rotate{};
		//	// Gamepad入力
		//	{
		//		const float margin = 0.8f;
		//		const float shortMaxReci = 1.0f / float(SHRT_MAX);
		//		rotate = { float(xinputState.Gamepad.sThumbLX), 0.0f, float(xinputState.Gamepad.sThumbLY) };
		//		rotate *= shortMaxReci;
		//		if (rotate.Length() < margin) {
		//			rotate = Vector3::zero;
		//		}
		//	}

		//	if(rotate != Vector3::zero) {
		//		rotate = workAttack_.playerRotate.Conjugate() * rotate;
		//		Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, rotate);
		//		workAttack_.playerRotate = Quaternion::Slerp(0.8f, Quaternion::identity, diff) * workAttack_.playerRotate;
		//	}

		//}

		if (++attack_.attackTimer >= WA_01_.allFrame) {

			//コンボ継続中なら次の攻撃を出す
			if (attack_.isCombo_) {

				attack_.currentCombo_++;

				attack_.isCombo_ = false;

				attack_.attackType = kHorizontal_2;

				ui_A_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

				BehaviorAttackInitialize();

			}
			//コンボ継続中でなかったら通常状態に戻る
			else {

				weapon_->SetDefault();
				/*weapon_->modelBodyTransform_.rotate = Quaternion::MakeFromTwoVector(Vector3::unitZ, Vector3{ 0.5f,0.5f,0.5f }) *
					Quaternion::identity;*/
				playerTransforms_[kHip]->rotate = attack_.playerRotate;
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

		//一定時間すぎた後、条件が揃っている状態で入力したら次のコンボ用意
		if (attack_.attackTimer >= WA_02_.allFrame / 4) {

			/*if (weapon_->GetIsGravity()) {
				ui_A_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
			}*/

			ui_A_->SetColor({ 1.0f,0.0f,0.0f,1.0f });

			if (/*weapon_->GetIsGravity() && */!attack_.isCombo_ &&
				attack_.currentCombo_ < 2 &&
				(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
				!(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
				attack_.isCombo_ = true;
			}

		}

		//攻撃中
		if (attack_.attackTimer < WA_02_.attackFrame) {
			playerTransforms_[kHip]->rotate = Quaternion::Slerp(float(1.0f / WA_02_.attackFrame),
				Quaternion::identity, Quaternion::MakeForYAxis(WA_02_.attackRotate)) * playerTransforms_[kHip]->rotate;
			weapon_->GetCollider()->SetIsActive(true);
			weapon_->isAttack_ = true;
		}
		else {
			weapon_->GetCollider()->SetIsActive(false);
			weapon_->isAttack_ = false;
		}

		if (++attack_.attackTimer >= WA_02_.allFrame) {

			//コンボ継続中なら次の攻撃を出す
			if (attack_.isCombo_) {

				attack_.currentCombo_++;

				attack_.isCombo_ = false;

				attack_.attackType = kRotateAttack;

				ui_A_->SetColor({ 1.0f,1.0f,1.0f,0.5f });

				BehaviorAttackInitialize();

			}
			//コンボ継続中でなかったら通常状態に戻る
			else {

				weapon_->SetDefault();
				/*weapon_->modelBodyTransform_.rotate = Quaternion::MakeFromTwoVector(Vector3::unitZ, Vector3{ 0.5f,0.5f,0.5f }) *
					Quaternion::identity;*/
				playerTransforms_[kHip]->rotate = attack_.playerRotate;
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

		//攻撃中
		if (attack_.attackTimer < WA_03_.attackFrame) {

			/*playerTransforms_[kHip]->translate += WA_03_.velocity;*/

			playerTransforms_[kHip]->rotate = Quaternion::Slerp(float(1.0f / (WA_03_.attackFrame / 4)),
				Quaternion::identity, Quaternion::MakeForYAxis(WA_03_.attackRotate)) * playerTransforms_[kHip]->rotate;

			weapon_->GetCollider()->SetIsActive(true);
			weapon_->isAttack_ = true;
		}
		//ジャンプ中
		else if (attack_.attackTimer < WA_03_.attackFrame + WA_03_.jumpFrame) {
			playerTransforms_[kHip]->translate.y += 1.0f;
			weapon_->GetCollider()->SetIsActive(true);
			weapon_->isAttack_ = true;
		}
		//ジャンプ待機
		else if (attack_.attackTimer < WA_03_.attackFrame + WA_03_.jumpFrame + WA_03_.waitFrameJump) {
			weapon_->GetCollider()->SetIsActive(true);
			weapon_->isAttack_ = true;
		}
		//落下中
		else if (attack_.attackTimer < WA_03_.attackFrame + WA_03_.jumpFrame
			+ WA_03_.waitFrameJump + WA_03_.fallFrame) {
			playerTransforms_[kHip]->translate.y -= 1.0f;
			weapon_->GetCollider()->SetIsActive(true);
			weapon_->isAttack_ = true;
		}
		//落下後衝撃波
		else if (attack_.attackTimer < WA_03_.attackFrame + WA_03_.jumpFrame
			+ WA_03_.waitFrameJump + WA_03_.fallFrame + WA_03_.shockWaveFrame) {
			weapon_->isShockWave_ = true;
			weapon_->shockWaveCollider_->SetIsActive(true);
			weapon_->GetCollider()->SetIsActive(true);
			weapon_->isAttack_ = true;

		}
		else {
			weapon_->isShockWave_ = false;
			weapon_->shockWaveCollider_->SetIsActive(false);
			weapon_->GetCollider()->SetIsActive(false);
			weapon_->isAttack_ = false;
		}

		if (++attack_.attackTimer >= WA_03_.allFrame) {

			//コンボ継続中でなかったら通常状態に戻る
			{

				weapon_->SetDefault();
				/*weapon_->modelBodyTransform_.rotate = Quaternion::MakeFromTwoVector(Vector3::unitZ, Vector3{ 0.5f,0.5f,0.5f }) *
					Quaternion::identity;*/
				playerTransforms_[kHip]->rotate = attack_.playerRotate;
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

	}

}

void Player::BehaviorAttackInitialize() {
	
	attack_.attackTimer = 0;
	attack_.isCombo_ = false;
	weapon_->isHit_ = false;
	weapon_->GetGravityCollider()->SetName("Gravity_Attack");

	switch (attack_.attackType)
	{
	default:
	case AttackType::kHorizontal_1:

		attack_.playerRotate = playerTransforms_[kHip]->rotate;

		weapon_->modelBodyTransform_->translate = { 0.0f,-2.0f,0.0f };

		break;
	case AttackType::kHorizontal_2:


		break;
	case AttackType::kRotateAttack:
		
		WA_03_.velocity = { 0.0f,0.0f,1.0f };

		WA_03_.velocity = WA_03_.velocity = attack_.playerRotate * WA_03_.velocity;
		

		break;

	}

	

}

void Player::BehaviorShotUpdate() {

	if (workShot_.shotTimer < 15) {

		//武器を持っている腕を動かす
		playerTransforms_[kRightUpperArm]->rotate = Quaternion::Slerp(1.0f / 15.0f, Quaternion::identity,
			Quaternion::MakeFromAngleAxis(workShot_.shotRotate, Vector3{ 1.0f,0.0f,0.0f }.Normalized())) * playerTransforms_[kRightUpperArm]->rotate;

	}
	//else if (workShot_.shotTimer > 45) {

	//	//武器を持っている腕を動かす
	//	playerTransforms_[kRightLowerArm]->rotate = Quaternion::Slerp(1.0f / 15.0f, Quaternion::identity,
	//		Quaternion::MakeFromAngleAxis(workShot_.shotRotate, Vector3{ 1.0f,0.5f,0.5f }.Normalized())) * playerTransforms_[kRightLowerArm]->rotate;

	//}

	if (++workShot_.shotTimer >= workShot_.maxShotFrame) {

		playerTransforms_[kRightUpperArm]->rotate = Quaternion::identity;

		workShot_.shotTimer = 0;

		behaviorRequest_ = Behavior::kRoot;

	}

}

void Player::BehaviorShotInitialize() {

	workShot_.shotTimer = 0;

}

void Player::KnockBack(const Vector3& affectPosition) {

	knockBackVelocity_ = affectPosition - transform.worldMatrix.GetTranslate();
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

	}
	else if (collisionInfo.collider->GetName() == "Barrier_Bullet") {

		auto object = collisionInfo.collider->GetGameObject();

		auto bullet = BarrierBulletManager::GetInstance()->GetBullet(object);

		Damage(1, bullet->transform.worldMatrix.GetTranslate());

	}

}

constexpr char kGroupName[] = "Player";

void Player::RegisterGlobalVariables() {
	
	GlobalVariables& globalVariables = *GlobalVariables::GetInstance();
	
	if (!globalVariables.HasGroup(kGroupName)) {
		auto& group = globalVariables[kGroupName];
		group["Dush Speed"] = workDash_.speed_;
		group["Attack PreFrame"] = WA_01_.preFrame;
		group["Attack WaitFrameBefore"] = WA_01_.waitFrameBefore;
		group["Attack WaitFrameAfter"] = WA_01_.waitFrameAfter;
		group["Attack AttackFrame"] = WA_01_.attackFrame;
		group["Attack PreRotate"] = WA_01_.preRotate;
		group["Attack AttackRotate"] = WA_01_.attackRotate;
		group["Attack_2 AttackFrame"] = WA_02_.attackFrame;
		group["Attack_2 WaitFrameAfter"] = WA_02_.waitFrameAfter;
		group["Attack_3 AttackFrame"] = WA_03_.attackFrame;
		group["Attack_3 JumpFrame"] = WA_03_.jumpFrame;
		group["Attack_3 WaitFrameJump"] = WA_03_.waitFrameJump;
		group["Attack_3 FallFrame"] = WA_03_.fallFrame;
		group["Attack_3 WaitFrameAfter"] = WA_03_.waitFrameAfter;

	}

}

void Player::ApplyGlobalVariables() {

	//攻撃中は変数を変更しない
	if (behavior_ != Behavior::kAttack) {

		GlobalVariables& globalVariables = *GlobalVariables::GetInstance();

		auto& group = globalVariables[kGroupName];

		workDash_.speed_ = group["Dush Speed"].Get<float>();
		WA_01_.preFrame = group["Attack PreFrame"].Get<int32_t>() + weapon_->GetDelay();
		WA_01_.waitFrameBefore = group["Attack WaitFrameBefore"].Get<int32_t>() + weapon_->GetDelay();
		WA_01_.waitFrameAfter = group["Attack WaitFrameAfter"].Get<int32_t>() + weapon_->GetDelay();
		WA_01_.attackFrame = group["Attack AttackFrame"].Get<int32_t>();
		WA_01_.preRotate = group["Attack PreRotate"].Get<float>();
		WA_01_.attackRotate = group["Attack AttackRotate"].Get<float>();
		WA_02_.attackFrame = group["Attack_2 AttackFrame"].Get<int32_t>();
		WA_02_.waitFrameAfter = group["Attack_2 WaitFrameAfter"].Get<int32_t>();
		WA_03_.attackFrame = group["Attack_3 AttackFrame"].Get<int32_t>();
		WA_03_.jumpFrame = group["Attack_3 JumpFrame"].Get<int32_t>();
		WA_03_.waitFrameJump = group["Attack_3 WaitFrameJump"].Get<int32_t>();
		WA_03_.fallFrame = group["Attack_3 FallFrame"].Get<int32_t>();
		WA_03_.waitFrameAfter = group["Attack_3 WaitFrameAfter"].Get<int32_t>();

		//攻撃に使う合計フレームを設定
		WA_01_.allFrame = WA_01_.preFrame + WA_01_.waitFrameBefore +
			WA_01_.attackFrame + WA_01_.waitFrameAfter;

		WA_02_.allFrame = WA_02_.attackFrame + WA_02_.waitFrameAfter;

		WA_03_.allFrame = WA_03_.attackFrame + WA_03_.jumpFrame + WA_03_.waitFrameJump +
			WA_03_.fallFrame + WA_03_.waitFrameAfter;

	}

}
