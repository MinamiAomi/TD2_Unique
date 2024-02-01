#include "Weapon.h"
#include "Graphics/ResourceManager.h"
#include "Player.h"
#include "Game/enemy/SmallEnemyManager.h"

Weapon::Weapon()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Weapon_Head"));
	modelBody_ = std::make_shared<ModelInstance>();
	modelBody_->SetModel(ResourceManager::GetInstance()->FindModel("Weapon"));
	gravityModel_ = std::make_shared<ModelInstance>();
	gravityModel_->SetModel(ResourceManager::GetInstance()->FindModel("Sphere"));
	collider_ = std::make_unique<BoxCollider>();
	spaceCollider_ = std::make_unique<SphereCollider>();
	gravityCollider_ = std::make_unique<SphereCollider>();
	gravitySpaceCollider_ = std::make_unique<SphereCollider>();
	gravityTransform_ = std::make_shared<Transform>();
	gravityScaleTransform_ = std::make_shared<Transform>();
	modelBodyTransform_ = std::make_shared<Transform>();
}

Weapon::~Weapon()
{
}

void Weapon::SetDefault() {

	modelBodyTransform_->translate = { 0.0f,-3.0f,0.0f };
	modelBodyTransform_->scale = Vector3::one * 0.5f;
	//modelBodyTransform_->rotate = Quaternion::MakeFromAngleAxis(-1.57f, Vector3{ 0.5f,1.0f,0.5f }.Normalized()) * Quaternion::identity;
	modelBodyTransform_->rotate = Quaternion::identity;
	modelBodyTransform_->UpdateMatrix();
	transform.SetParent(modelBodyTransform_.get());
	transform.translate = { 0.0f,7.0f,0.0f };
	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	transform.UpdateMatrix();
	
	if (isGravity_) {
		gravityCollider_->SetName("Gravity");
	}

}

void Weapon::Initialize() {

	SetName("Weapon");

	SetDefault();

	gravityTransform_->SetParent(&transform);
	gravityTransform_->translate = Vector3::zero;
	gravityTransform_->scale = Vector3::one;
	gravityTransform_->rotate = Quaternion::identity;
	gravityTransform_->UpdateMatrix();

	gravityScaleTransform_->SetParent(gravityTransform_.get());
	gravityScaleTransform_->translate = Vector3::zero;
	gravityScaleTransform_->scale = Vector3::one;
	gravityScaleTransform_->rotate = Quaternion::identity;

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Weapon");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetGameObject(this);
	collider_->SetIsActive(false);
	collider_->SetCollisionAttribute(0xfffffffe);
	collider_->SetCollisionMask(0x00000001);

	spaceCollider_->SetCenter(transform.translate);
	spaceCollider_->SetRadius(1.0f);
	spaceCollider_->SetName("Weapon");
	spaceCollider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	spaceCollider_->SetGameObject(this);
	spaceCollider_->SetIsActive(false);
	spaceCollider_->SetCollisionAttribute(0xfffffffe);
	spaceCollider_->SetCollisionMask(0x00000001);

	gravityCollider_->SetCenter(gravityTransform_->translate);
	gravityCollider_->SetRadius(gravityTransform_->scale.x);
	gravityCollider_->SetName("Gravity");
	gravityCollider_->SetCallback([this](const CollisionInfo& collisionInfo) {GravityOnCollision(collisionInfo); });
	gravityCollider_->SetGameObject(this);
	gravityCollider_->SetCollisionAttribute(0xfffffffe);
	gravityCollider_->SetCollisionMask(0x00000001);

	gravitySpaceCollider_->SetCenter(gravityTransform_->translate);
	gravitySpaceCollider_->SetRadius(2.0f);
	gravitySpaceCollider_->SetName("Gravity");
	gravitySpaceCollider_->SetCallback([this](const CollisionInfo& collisionInfo) {GravityOnCollision(collisionInfo); });
	gravitySpaceCollider_->SetGameObject(this);
	gravitySpaceCollider_->SetCollisionAttribute(0xfffffffe);
	gravitySpaceCollider_->SetCollisionMask(0x00000001);

	gravityModel_->SetIsActive(false);

	energyCount_ = 0;

	isShot_ = false;
	isBreak_ = false;
	isGravity_ = false;

}

void Weapon::Update() {

	if (energyCount_ > 20) {
		energyCount_ = 20;
	}

	//重力波が破裂する時
	if (isBreak_) {

		if (--breakTimer_ <= 0) {
			Reset();
		}

	}
	//重力波が発射されている途中
	else if (isShot_) {

		transform.translate += velocity_;

		if (--shotTimer_ <= 0) {
			Break();
		}

	}
	else {

		

	}

	if (isGravity_) {
		gravityTransform_->rotate = Quaternion::MakeFromAngleAxis(0.1f, Vector3{ 0.5f,0.5f,0.5f }.Normalized()) *
			gravityTransform_->rotate;
	}

	if (isAttack_ && !isGravity_) {
		collider_->SetIsActive(true);
		spaceCollider_->SetIsActive(true);
		model_->SetColor({ 1.0f,0.0f,0.0f });
	}
	else {
		collider_->SetIsActive(false);
		spaceCollider_->SetIsActive(false);
		model_->SetColor({ 1.0f,1.0f,1.0f });
	}

	if ((isThrust_ || isShot_ || isAttack_ || isBreak_) && isGravity_) {
		gravityCollider_->SetIsActive(true);
		gravitySpaceCollider_->SetIsActive(true);
		gravityModel_->SetIsActive(true);
		gravityModel_->SetColor({ 1.0f,0.0f,0.0f });
	}
	else {
		gravityCollider_->SetIsActive(false);
		gravitySpaceCollider_->SetIsActive(false);
		gravityModel_->SetColor({ 1.0f,1.0f,1.0f });
	}

	//重力波のレベルに応じて当たり判定を肥大化
	switch (gravityLevel_)
	{
	default:
	case Weapon::kSmall:
		gravityScaleTransform_->scale = { 3.0f,3.0f,3.0f };
		break;
	case Weapon::kMedium:
		gravityScaleTransform_->scale = { 5.0f,5.0f,5.0f };
		break;
	case Weapon::kWide:
		gravityScaleTransform_->scale = { 8.0f,8.0f,8.0f };
		break;
	}

	//重力をまとっている間
	if (isGravity_ && !isShot_) {
		gravityDelay_ = int32_t(gravityLevel_ * 5) + 5;
	}
	else {
		gravityDelay_ = 0;
	}

	gravityTransform_->UpdateMatrix();
	gravityScaleTransform_->UpdateMatrix();
	modelBodyTransform_->UpdateMatrix();
	transform.UpdateMatrix();

	gravityCollider_->SetCenter(gravityTransform_->worldMatrix.GetTranslate());
	gravityCollider_->SetRadius(gravityScaleTransform_->scale.x);
	gravitySpaceCollider_->SetCenter(player_->GetPosition() + (gravityTransform_->worldMatrix.GetTranslate() - player_->GetPosition()) / 2.0f);
	gravitySpaceCollider_->SetRadius(gravityScaleTransform_->scale.x / 2.0f);

	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetSize(transform.worldMatrix.GetScale() * 2.0f);
	collider_->SetOrientation(transform.worldMatrix.GetRotate());
	spaceCollider_->SetCenter(player_->GetPosition() + (transform.worldMatrix.GetTranslate() - player_->GetPosition()) / 2.0f);

	model_->SetWorldMatrix(transform.worldMatrix);
	modelBody_->SetWorldMatrix(modelBodyTransform_->worldMatrix);
	gravityModel_->SetWorldMatrix(gravityScaleTransform_->worldMatrix);

}

void Weapon::Shot(const Vector3& velocity) {

	transform.translate = transform.worldMatrix.GetTranslate();
	transform.SetParent(nullptr);

	velocity_ = velocity;
	velocity_.Normalize();
	
	velocity_ *= 6.0f;
	isShot_ = true;
	shotTimer_ = kMaxShotTime_;
	
	gravityCollider_->SetName("Gravity_Shot");
	
}

void Weapon::Break() {

	isBreak_ = true;
	isShot_ = false;

	breakTimer_ = kMaxBreakTime_;

	gravityModel_->SetColor({ 1.0f,0.0f,1.0f });
	gravityCollider_->SetName("Gravity_Break");
	
}

void Weapon::AddGravity() {

	isGravity_ = true;
	gravityLevel_ = kSmall;

}

void Weapon::Reset() {

	energyCount_ = 0;

	isShot_ = false;
	isBreak_ = false;
	isGravity_ = false;

	gravityModel_->SetColor({ 1.0f,1.0f,1.0f });
	gravityModel_->SetIsActive(false);

	gravityCollider_->SetName("Gravity");
	gravityCollider_->SetIsActive(false);
	gravityLevel_ = kSmall;
	
	SetDefault();

	transform.UpdateMatrix();

}

void Weapon::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy") {

		isHit_ = true;

	}
	else if (collisionInfo.collider->GetName() == "Small_Enemy" ||
		collisionInfo.collider->GetName() == "Barrier_Enemy") {

		auto object = collisionInfo.collider->GetGameObject();

		std::shared_ptr<SmallEnemy> enemy = SmallEnemyManager::GetInstance()->GetEnemy(object);

		enemy->Damage(1, player_->playerTransforms_[Player::kHip]->worldMatrix.GetTranslate());

	}

}

void Weapon::GravityOnCollision(const CollisionInfo& collisionInfo) {

	//重力をまとっている、発射している途中
	if (gravityCollider_->GetName() == "Gravity" || 
		gravityCollider_->GetName() == "Gravity_Shot") {

		if (collisionInfo.collider->GetName() == "Small_Enemy") {

			energyCount_++;

			auto object = collisionInfo.collider->GetGameObject();

			std::shared_ptr<SmallEnemy> enemy = SmallEnemyManager::GetInstance()->GetEnemy(object);

			enemy->transform.SetParent(gravityTransform_.get());
			enemy->transform.translate /= 2.0f;
			enemy->GetCollider()->SetName("Small_Enemy_Affected");

			if (energyCount_ >= 20) {
				gravityLevel_ = kWide;
			}
			else if (energyCount_ >= 10) {
				gravityLevel_ = kMedium;
			}
			else {
				gravityLevel_ = kSmall;
			}

			isHit_ = true;

		}
		//地面に衝突したらフレームカウントを0にして破裂
		else if (collisionInfo.collider->GetName() == "Stage") {
			shotTimer_ = 0;
		}

	}
	//発射して破裂した時
	else if (gravityCollider_->GetName() == "Gravity_Break") {

		if (collisionInfo.collider->GetName() == "Small_Enemy" ||
			collisionInfo.collider->GetName() == "Small_Enemy_Affected") {

			auto object = collisionInfo.collider->GetGameObject();

			std::shared_ptr<SmallEnemy> enemy = SmallEnemyManager::GetInstance()->GetEnemy(object);

			enemy->transform.SetParent(nullptr);
			enemy->Damage(3 + gravityLevel_, transform.worldMatrix.GetTranslate());

			isHit_ = true;

		}

	}
	//殴った時
	else if (gravityCollider_->GetName() == "Gravity_Attack") {

		if (collisionInfo.collider->GetName() == "Small_Enemy" ||
			collisionInfo.collider->GetName() == "Barrier_Enemy") {

			auto object = collisionInfo.collider->GetGameObject();

			std::shared_ptr<SmallEnemy> enemy = SmallEnemyManager::GetInstance()->GetEnemy(object);

			enemy->Damage(1 + gravityLevel_, player_->playerTransforms_[Player::kHip]->worldMatrix.GetTranslate());

		}

	}

	

}

void Weapon::GravityDamageOnCollision(const CollisionInfo& collisionInfo) {

	collisionInfo;

}
