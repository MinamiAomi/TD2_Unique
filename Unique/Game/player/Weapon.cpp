#include "Weapon.h"
#include "Graphics/ResourceManager.h"

Weapon::Weapon()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	collider_ = std::make_unique<BoxCollider>();
}

Weapon::~Weapon()
{
}

void Weapon::Initialize() {

	SetName("Weapon");

	transform.translate = Vector3::zero;
	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Weapon");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });

}

void Weapon::Update() {

	transform.UpdateMatrix();

	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetSize(transform.worldMatrix.GetScale() * 2.0f);
	collider_->SetOrientation(transform.worldMatrix.GetRotate());
	model_->SetWorldMatrix(transform.worldMatrix);

}

void Weapon::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy") {

		isHit_ = true;

	}

}
