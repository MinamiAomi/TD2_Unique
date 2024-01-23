#include "Player.h"
#include "Engine/input/Input.h"
#include "Graphics/ResourceManager.h"
#include "Game/block/block.h"
#include "GlobalVariables.h"

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
	ui_RT_Tex_ = ResourceManager::GetInstance()->FindTexture("UI_RT");

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
	
	transform.translate = Vector3::zero;
	transform.scale = Vector3::one;
	transform.scale.z = 1.0f;

	weapon_->Initialize();
	weapon_->transform.SetParent(&transform);
	weapon_->transform.translate = { 3.0f,1.0f,0.0f };
	weapon_->transform.scale = Vector3::one;
	/*weapon_->transform.rotate = Quaternion::MakeFromTwoVector(Vector3::unitZ, Vector3{ 0.5f,0.5f,0.5f }) *
		Quaternion::identity;*/
	weapon_->transform.UpdateMatrix();
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

	RegisterGlobalVariables();

}

void Player::Update() {

	ApplyGlobalVariables();

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

	reticle_->Update();

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

	if (!isDead_ ) {
		weapon_->GetModel()->SetIsActive(true);
	}
	else {
		weapon_->GetModel()->SetIsActive(false);
	}

}

void Player::BehaviorRootUpdate() {

	auto& camera = camera_->GetCamera();
	auto input = Input::GetInstance();

	auto& xinputState = input->GetXInputState();

	auto& preXInputState = input->GetPreXInputState();

	//重力付与、前に突き立て
	if ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && 
		!weapon_->GetIsShot()) {
		Thrust();
	}
	else {
		weapon_->isThrust_ = false;
	}

	//突き立て終了時
	if (!(xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
		(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
		!weapon_->GetIsShot()) {
		weapon_->transform.translate = { 3.0f,1.0f,0.0f };
	}

	//重力波発射
	if (xinputState.Gamepad.bRightTrigger) {

		//重力付与状態で発射していなかったら
		if (weapon_->GetIsGravity() && !weapon_->GetIsShot()) {
			weapon_->Shot(reticle_->GetReticlePosition() - weapon_->GetPosition());
		}

	}

	// 攻撃に遷移
	// 重力波発射中と突き出し中は遷移不可
	if (((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
		!(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) && 
		!weapon_->GetIsShot() && !weapon_->isThrust_) {


		workAttack_.attackType = kHorizontal;

		behaviorRequest_ = Behavior::kAttack;

	}
	// ダッシュ
	if ((xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		
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
			//transform.rotate = Quaternion::Slerp(0.2f, transform.rotate, Quaternion::MakeLookRotation(move));

			//武器を前に掲げている時は回転させない
			if (!weapon_->isThrust_) {
				move = transform.rotate.Conjugate() * move;
				Quaternion diff = Quaternion::MakeFromTwoVector(Vector3::unitZ, move);
				transform.rotate = Quaternion::Slerp(0.8f, Quaternion::identity, diff) * transform.rotate;
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

	//重力波が発射されていない時
	if (!weapon_->GetIsShot()) {

		//重力が付与されていなかったら重力付与
		if (!weapon_->GetIsGravity()) {
			weapon_->AddGravity();
		}

		weapon_->transform.translate = { 0.0f,1.0f,10.0f };
		weapon_->transform.rotate = Quaternion::identity;
		weapon_->isThrust_ = true;

	}
	else {
		weapon_->isThrust_ = false;
	}

}

void Player::BehaviorAttackUpdate() {

	switch (workAttack_.attackType)
	{
	default:
	case AttackType::kHorizontal:

		//攻撃中
		if (workAttack_.attackTimer >= workAttack_.waitFrameBefore + workAttack_.preFrame &&
			workAttack_.attackTimer - workAttack_.waitFrameBefore - workAttack_.preFrame < workAttack_.attackFrame) {
			transform.rotate = Quaternion::Slerp(float(1.0f / workAttack_.attackFrame),
				Quaternion::identity, Quaternion::MakeForYAxis(workAttack_.attackRotate)) * transform.rotate;
			weapon_->GetCollider()->SetIsActive(true);
			weapon_->isAttack_ = true;
		}
		//攻撃開始前
		else if (workAttack_.attackTimer < workAttack_.preFrame) {
			transform.rotate = Quaternion::Slerp(1.0f / float(workAttack_.preFrame),
				Quaternion::identity, Quaternion::MakeForYAxis(workAttack_.preRotate)) * transform.rotate;
			weapon_->GetCollider()->SetIsActive(false);
			weapon_->isAttack_ = false;
		}

		if (++workAttack_.attackTimer >= workAttack_.allFrame) {
			weapon_->transform.translate = { 3.0f,1.0f,0.0f };
			/*weapon_->transform.rotate = Quaternion::MakeFromTwoVector(Vector3::unitZ, Vector3{ 0.5f,0.5f,0.5f }) * 
				Quaternion::identity;*/
			transform.rotate = workAttack_.playerRotate;
			workAttack_.isAttack = false;
			weapon_->GetCollider()->SetIsActive(false);
			weapon_->isAttack_ = false;
			behaviorRequest_ = Behavior::kRoot;
		}

		break;
	/*case AttackType::kAddBlock:

		if (workAttack_.attackTimer == 0) {
			blocks_.clear();

			std::shared_ptr<Block> block = std::make_shared<Block>();
			block->Initialize(this->GetNewBlockPosition(),
				this, { 4.0f,5.0f,2.0f });
			blocks_.push_back(block);
		}

		if (++workAttack_.attackTimer >= workAttack_.allFrame) {
			weapon_->transform.translate = { 0.0f,3.0f,3.0f };
			workAttack_.isAttack = false;
			behaviorRequest_ = Behavior::kRoot;
		}

		break;*/
	}

}

void Player::BehaviorAttackInitialize() {
	
	workAttack_.attackTimer = 0;
	workAttack_.isAttack = true;
	workAttack_.isHit = false;
	workAttack_.playerRotate = transform.rotate;
	weapon_->isHit_ = false;

	switch (workAttack_.attackType)
	{
	default:
	case AttackType::kHorizontal:

		

		if (workAttack_.preFrame == 0) {
			weapon_->transform.translate = { 10.0f,1.0f,0.0f };
		}
		else {
			weapon_->transform.translate = { 0.0f,1.0f,10.0f };
		}

		workAttack_.velocity = { 0.4f,0.0f,0.0f };

		break;
	/*case AttackType::kAddBlock:
		workAttack_.isAttack = false;
		break;*/
	}

	

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

constexpr char kGroupName[] = "Player";

void Player::RegisterGlobalVariables() {
	
	GlobalVariables& globalVariables = *GlobalVariables::GetInstance();
	
	if (!globalVariables.HasGroup(kGroupName)) {
		auto& group = globalVariables[kGroupName];
		group["Dush Speed"] = workDash_.speed_;
		group["Attack PreFrame"] = workAttack_.preFrame;
		group["Attack WaitFrameBefore"] = workAttack_.waitFrameBefore;
		group["Attack WaitFrameAfter"] = workAttack_.waitFrameAfter;
		group["Attack AttackFrame"] = workAttack_.attackFrame;
		group["Attack PreRotate"] = workAttack_.preRotate;
		group["Attack AttackRotate"] = workAttack_.attackRotate;
	}

}

void Player::ApplyGlobalVariables() {

	//攻撃中は変数を変更しない
	if (!workAttack_.isAttack) {

		GlobalVariables& globalVariables = *GlobalVariables::GetInstance();

		auto& group = globalVariables[kGroupName];

		workDash_.speed_ = group["Dush Speed"].Get<float>();
		workAttack_.preFrame = group["Attack PreFrame"].Get<int32_t>() + weapon_->GetDelay();
		workAttack_.waitFrameBefore = group["Attack WaitFrameBefore"].Get<int32_t>() + weapon_->GetDelay();
		workAttack_.waitFrameAfter = group["Attack WaitFrameAfter"].Get<int32_t>() + weapon_->GetDelay();
		workAttack_.attackFrame = group["Attack AttackFrame"].Get<int32_t>();
		workAttack_.preRotate = group["Attack PreRotate"].Get<float>();
		workAttack_.attackRotate = group["Attack AttackRotate"].Get<float>();

		//攻撃に使う合計フレームを設定
		workAttack_.allFrame = workAttack_.preFrame + workAttack_.waitFrameBefore +
			workAttack_.attackFrame + workAttack_.waitFrameAfter;

	}

}
