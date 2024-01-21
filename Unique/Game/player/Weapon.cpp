#include "Weapon.h"
#include "Graphics/ResourceManager.h"
#include "Player.h"

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

	gravityCollider_->SetCenter(gravityTransform_->translate);
	gravityCollider_->SetRadius(gravityTransform_->scale.x);
	gravityCollider_->SetName("Gravity");
	gravityCollider_->SetCallback([this](const CollisionInfo& collisionInfo) {GravityOnCollision(collisionInfo); });
	gravityCollider_->SetGameObject(this);

}

void Weapon::Update() {

	if (energyCount_ > 20) {
		energyCount_ = 20;
	}

	if (isShot_) {

		transform.translate += velocity_;

		

		if (--shotTimer_ <= 0) {
			Break();
		}

	}
	else {

		

	}

	transform.UpdateMatrix();

	if (isThrust_) {
		gravityCollider_->SetIsActive(true);
		gravityModel_->SetIsActive(true);
		collider_->SetIsActive(false);
	}
	else {
		gravityCollider_->SetIsActive(false);
		gravityModel_->SetIsActive(false);
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

	if (player_) {
		velocity_ = player_->transform.rotate * velocity_;
	}

	velocity_ *= 3.0f;
	isShot_ = true;
	shotTimer_ = kMaxShotTime_;
	
	gravityCollider_->SetName("Gravity_Shot");

}

void Weapon::Break() {

	energyCount_ = 0;

	isShot_ = false;
	isGravity_ = false;

	Reset();

}

void Weapon::AddGravity() {

	isGravity_ = true;
	gravityLevel_ = kSmall;

}

void Weapon::Reset() {

	transform.translate = {0.0f,0.0f,3.0f};

}

void Weapon::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy") {

		isHit_ = true;

	}

}

void Weapon::GravityOnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy_Core" ||
		collisionInfo.collider->GetName() == "Enemy_Bullet" ||
		collisionInfo.collider->GetName() == "Small_Enemy") {

		energyCount_++;

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
