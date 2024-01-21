#include "SmallEnemy.h"
#include "Graphics/ResourceManager.h"
#include "SmallEnemyManager.h"

SmallEnemy::SmallEnemy()
{

	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	collider_ = std::make_unique<BoxCollider>();

}

SmallEnemy::~SmallEnemy()
{
}

void SmallEnemy::Initialize(const Vector3& startPosition) {

	SetName("Small_Enemy");

	transform.translate = startPosition;
	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale);
	collider_->SetOrientation(transform.rotate);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetName("Small_Enemy");
	collider_->SetGameObject(this);

}

void SmallEnemy::Update() {

	if (hp_ <= 0) {
		isDead_ = true;
	}

	transform.UpdateMatrix();

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale);
	collider_->SetOrientation(transform.rotate);

	model_->SetWorldMatrix(transform.worldMatrix);

}

void SmallEnemy::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Gravity" ||
		collisionInfo.collider->GetName() == "Gravity_Shot") {

		collider_->SetIsActive(false);

	}

}
