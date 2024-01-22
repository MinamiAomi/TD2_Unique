#include "Weapon.h"
#include "Graphics/ResourceManager.h"
#include "Player.h"
#include "Game/enemy/SmallEnemyManager.h"

Weapon::Weapon()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	gravityModel_ = std::make_shared<ModelInstance>();
	gravityModel_->SetModel(ResourceManager::GetInstance()->FindModel("Sphere"));
	collider_ = std::make_unique<BoxCollider>();
	gravityCollider_ = std::make_unique<SphereCollider>();
	gravityTransform_ = std::make_shared<Transform>();
}

Weapon::~Weapon()
{
}

void Weapon::Initialize() {

	SetName("Weapon");

	transform.translate = Vector3::zero;
	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;

	gravityTransform_->translate = Vector3::zero;
	gravityTransform_->scale = Vector3::one;
	gravityTransform_->rotate = Quaternion::identity;
	gravityTransform_->SetParent(&transform);

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Weapon");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetGameObject(this);
	collider_->SetIsActive(false);

	gravityCollider_->SetCenter(gravityTransform_->translate);
	gravityCollider_->SetRadius(gravityTransform_->scale.x);
	gravityCollider_->SetName("Gravity");
	gravityCollider_->SetCallback([this](const CollisionInfo& collisionInfo) {GravityOnCollision(collisionInfo); });
	gravityCollider_->SetGameObject(this);

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

	transform.UpdateMatrix();

	if ((isThrust_ || isShot_ || isAttack_) && isGravity_) {
		gravityCollider_->SetIsActive(true);
		gravityModel_->SetIsActive(true);
		collider_->SetIsActive(false);
	}
	else {
		gravityCollider_->SetIsActive(false);
	}

	//重力波のレベルに応じて当たり判定を肥大化
	switch (gravityLevel_)
	{
	default:
	case Weapon::kSmall:
		gravityTransform_->scale = { 3.0f,3.0f,3.0f };
		break;
	case Weapon::kMedium:
		gravityTransform_->scale = { 5.0f,5.0f,5.0f };
		break;
	case Weapon::kWide:
		gravityTransform_->scale = { 8.0f,8.0f,8.0f };
		break;
	}

	//重力をまとっている間
	if (isGravity_ && !isShot_) {
		gravityDelay_ = int32_t(gravityLevel_ * 5);
	}
	else {
		gravityDelay_ = 0;
	}

	gravityTransform_->UpdateMatrix();

	gravityCollider_->SetCenter(gravityTransform_->worldMatrix.GetTranslate());
	gravityCollider_->SetRadius(gravityTransform_->scale.x);

	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetSize(transform.worldMatrix.GetScale() * 2.0f);
	collider_->SetOrientation(transform.worldMatrix.GetRotate());
	model_->SetWorldMatrix(transform.worldMatrix);
	gravityModel_->SetWorldMatrix(gravityTransform_->worldMatrix);

}

void Weapon::Shot(const Vector3& velocity) {

	transform.translate = transform.worldMatrix.GetTranslate();
	transform.SetParent(nullptr);

	velocity_ = velocity;
	velocity_.Normalize();

	velocity_ *= 3.0f;
	isShot_ = true;
	shotTimer_ = kMaxShotTime_;
	
	gravityCollider_->SetName("Gravity_Shot");
	
}

void Weapon::Break() {

	isBreak_ = true;

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
	
	transform.SetParent(&player_->transform);

	transform.translate = { 0.0f,0.0f,3.0f };
	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	transform.UpdateMatrix();

}

void Weapon::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy") {

		isHit_ = true;

	}
	else if (collisionInfo.collider->GetName() == "Small_Enemy") {

		auto object = collisionInfo.collider->GetGameObject();

		std::shared_ptr<SmallEnemy> enemy = SmallEnemyManager::GetInstance()->GetEnemy(object);

		enemy->Damage(0, player_->transform.worldMatrix.GetTranslate());

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

	

}

void Weapon::GravityDamageOnCollision(const CollisionInfo& collisionInfo) {

	collisionInfo;

}
