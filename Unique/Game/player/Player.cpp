#include "Player.h"
#include "Engine/input/Input.h"
#include "Graphics/ResourceManager.h"
#include "Game/block/block.h"

Player::Player()
{

	playerModel_ = std::make_shared<ModelInstance>();
	playerModel_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	hpTex_ = ResourceManager::GetInstance()->FindTexture("player_hp");

	hpSprite_ = std::make_unique<Sprite>();
	hpSprite_->SetTexture(hpTex_);
	hpSprite_->SetPosition({ 640.0f,10.0f });

	ui_A_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_A");
	ui_RB_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_RB");
	ui_LB_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_LB");

	ui_A_ = std::make_unique<Sprite>();
	ui_A_->SetTexture(ui_A_Tex_);
	ui_A_->SetPosition({ 1100.0f,150.0f });
	ui_A_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
	ui_A_->SetScale({ 256.0f,64.0f });

	ui_RB_ = std::make_unique<Sprite>();
	ui_RB_->SetTexture(ui_RB_Tex_);
	ui_RB_->SetPosition({ 1000.0f,50.0f });
	ui_RB_->SetTexcoordRect({ 512.0f,64.0f }, { 512.0f,64.0f });
	ui_RB_->SetScale({ 512.0f,64.0f });

	ui_LB_ = std::make_unique<Sprite>();
	ui_LB_->SetTexture(ui_LB_Tex_);
	ui_LB_->SetPosition({ 1100.0f,250.0f });
	ui_LB_->SetTexcoordRect({ 256.0f,64.0f }, { 256.0f,64.0f });
	ui_LB_->SetScale({ 256.0f,64.0f });

	collider_ = std::make_unique<BoxCollider>();

	weapon_ = std::make_unique<Weapon>();

}

Player::~Player()
{
}

void Player::Initialize() {

	SetName("Player");

	input_ = Input::GetInstance();
	
	transform.translate = Vector3::zero;
	transform.scale = Vector3::one;
	transform.scale.z = 2.0f;

	weapon_->Initialize();
	weapon_->transform.SetParent(&transform);
	weapon_->transform.translate = { 0.0f,3.0f,3.0f };
	weapon_->transform.scale = Vector3::one;
	weapon_->transform.UpdateMatrix();

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

	velocity_ = { 0.0f,0.0f,1.0f };

	behavior_ = Behavior::kRoot;

	workDash_.dashParamater_ = 0;
	workDash_.speed_ = 2.0f;
	workDash_.dashTime_ = 5;

	/*workAttack_.attackFrame = 30;*/
	workAttack_.attackTimer = 0;
	workAttack_.attackType = kVertical;
	workAttack_.velocity = { 0.0f,0.0f,0.0f };

	workInvincible_.invincibleTimer = 0;
	workInvincible_.isInvincible = false;

	hpSprite_->SetScale({ 10.0f * hp_, 64.0f });

	dashSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/dash.wav");
	deathSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/disolve.wav");
	shootSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/shoot.wav");
	crashSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/proto_sound/crash.wav");

	blocks_.clear();

}

void Player::Update() {

	blocks_.remove_if([](auto& block) {

		if (block->GetIsDead()) {
			return true;
		}

		return false;

	});

	prePosition_ = Vector3{
			transform.worldMatrix.m[3][0],
			transform.worldMatrix.m[3][1],
			transform.worldMatrix.m[3][2] };

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
			case Behavior::kDash:
				BehaviorDashInitialize();
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
		case Behavior::kDash:
			BehaviorDashUpdate();
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

	for (auto& block : blocks_) {
		block->Update();
	}

	weapon_->Update();

	transform.UpdateMatrix();
	collider_->SetCenter(transform.translate);
	collider_->SetOrientation(transform.rotate);
	playerModel_->SetWorldMatrix(transform.worldMatrix);

	if (!isDead_ && workInvincible_.invincibleTimer % 2 == 0) {
		playerModel_->SetIsActive(true);
	}
	else {
		playerModel_->SetIsActive(false);
	}

	if (!isDead_ && workAttack_.isAttack) {
		weapon_->GetModel()->SetIsActive(true);
		weapon_->GetCollider()->SetIsActive(true);
	}
	else {
		weapon_->GetModel()->SetIsActive(false);
		weapon_->GetCollider()->SetIsActive(false);
	}

}

void Player::BehaviorRootUpdate() {

	auto& camera = camera_->GetCamera();
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
			move = move.Normalized() * 0.7f;

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


	auto& xinputState = input->GetXInputState();

	auto& preXInputState = input->GetPreXInputState();

	//ボタンを押している時
	if ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {

		if (inputTime_ < 30) {
			inputTime_++;
		}
		
	}

	// 攻撃に遷移(離した時、または既定の時間長押しした時)
	if ((!(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && (preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) ||
		inputTime_ >= 30) {


		//単押しなら通常振り
		if (inputTime_ < 30) {
			workAttack_.attackType = kHorizontal;
		}
		//長押しなら壁生成
		else {
			workAttack_.attackType = kAddBlock;
		}

		behaviorRequest_ = Behavior::kAttack;
		inputTime_ = 0;

	}
	// ダッシュに遷移
	else if ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) && !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		behaviorRequest_ = Behavior::kDash;
		Audio::GetInstance()->SoundPlayWave(dashSE_);
	}

	/*if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		workAttack_.attackType = kHorizontal;
		behaviorRequest_ = Behavior::kAttack;
	}*/

}

void Player::BehaviorRootInitialize() {
	
}

void Player::BehaviorAttackUpdate() {

	switch (workAttack_.attackType)
	{
	default:
	case AttackType::kVertical:

		if (workAttack_.attackTimer < workAttack_.waitFrame) {
			weapon_->transform.translate += workAttack_.velocity;
		}

		break;
	case AttackType::kHorizontal:

		if (workAttack_.attackTimer < workAttack_.waitFrame) {
			weapon_->transform.translate += workAttack_.velocity;
		}

		break;
	case AttackType::kAddBlock:

		if (workAttack_.attackTimer == 0) {
			blocks_.clear();

			std::shared_ptr<Block> block = std::make_shared<Block>();
			block->Initialize(this->GetNewBlockPosition(),
				this, { 4.0f,5.0f,2.0f });
			blocks_.push_back(block);
		}

		break;
	}

	if (++workAttack_.attackTimer == workAttack_.attackFrame) {
		weapon_->transform.translate = { 0.0f,3.0f,3.0f };
		workAttack_.isAttack = false;
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Player::BehaviorAttackInitialize() {
	
	workAttack_.attackTimer = 0;
	workAttack_.isAttack = true;
	workAttack_.isHit = false;
	weapon_->isHit_ = false;

	switch (workAttack_.attackType)
	{
	default:
	case AttackType::kVertical:

		weapon_->transform.translate = { 0.0f,3.0f,0.0f };
		workAttack_.velocity = { 0.0f,-0.4f,0.3f };

		break;
	case AttackType::kHorizontal:

		weapon_->transform.translate = { -2.0f,3.0f,4.0f };
		workAttack_.velocity = { 0.4f,0.0f,0.0f };

		break;
	case AttackType::kAddBlock:
		workAttack_.isAttack = false;
		break;
	}

	

}

void Player::Attack() {


}


void Player::BehaviorDashUpdate() {

	auto move = transform.rotate.GetForward() * workDash_.speed_;
	transform.translate += move;

	//ダッシュ時間
	const uint32_t behaviorDashTime = workDash_.dashTime_;

	//既定の時間で通常行動に戻る
	if (++workDash_.dashParamater_ >= behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Player::BehaviorDashInitialize() {

	workDash_.dashParamater_ = 0;

}

void Player::Damage(uint32_t val) {

	if (!workInvincible_.isInvincible) {
		hp_ -= val;
		workInvincible_.invincibleTimer = 60;
		workInvincible_.isInvincible = true;
	}

}

void Player::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy_Bullet") {

		Damage(1);

	}

}
