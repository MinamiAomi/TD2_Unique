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
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetName("Small_Enemy");
	collider_->SetGameObject(this);
	collider_->SetCollisionAttribute(0xfffffffd);
	collider_->SetCollisionMask(0x00000002);

}

void SmallEnemy::Update() {

	if (hp_ <= 0) {

		transform.translate += velocity_;

		if (--deadCount_ <= 0) {
			isDead_ = true;
		}

	}
	else {

		transform.translate += velocity_;

		velocity_ /= 1.05f;

		if (velocity_.Length() < 0.05f) {
			velocity_ = Vector3::zero;
		}

	}

	transform.UpdateMatrix();

	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);

	model_->SetWorldMatrix(transform.worldMatrix);

}

void SmallEnemy::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Gravity") {

		

	}
	else if (collisionInfo.collider->GetName() == "Gravity_Shot") {



	}
	else if (collisionInfo.collider->GetName() == "Gravity_Break") {



	}

}

void SmallEnemy::Damage(uint32_t val, const Vector3& affectPosition) {

	hp_ -= val;

	if (hp_ < 0) {
		hp_ = 0;
	}

	//攻撃を受けた地点からノックバック
	velocity_ = transform.worldMatrix.GetTranslate() - affectPosition;

	velocity_.y = 0.0f;

	velocity_ = velocity_.Normalized() * 1.5f;

}
