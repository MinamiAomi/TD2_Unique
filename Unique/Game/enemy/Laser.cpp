#include "Laser.h"
#include "Graphics/ResourceManager.h"

Laser::Laser()
{

	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	collider_ = std::make_unique<BoxCollider>();

}

Laser::~Laser()
{
}

void Laser::Initialize() {

	transform.translate = Vector3::zero;
	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;
	transform.UpdateMatrix();
	
	model_->SetWorldMatrix(transform.worldMatrix);

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Enemy_Bullet");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetGameObject(this);

}

void Laser::Update() {

	//テストコード。レーザーの伸ばし方
	/*for (uint32_t i = 0; i < 3; i++) {
		transforms_[i].translate = { 0.0f + float(i),3.0f, float(i * 2.0f) };
		transforms_[i].scale = { 1.0f + float(i),1.0f,1.0f };
		transforms_[i].rotate = Quaternion::identity;
		transforms_[i].UpdateMatrix();
		testModels_[i]->SetWorldMatrix(transforms_[i].worldMatrix);
	}*/

	if (isShot_) {


		if (liveTime_ > liveTime_ / 2) {
			transform.translate += velocity_;
			transform.scale.z += speed_;
		}
		else {
			transform.translate += velocity_;
			transform.scale.z -= speed_;
		}

		if (--liveTime_ <= 0) {
			isDead_ = true;
		}

	}
	else {

	}

	transform.UpdateMatrix();

	model_->SetWorldMatrix(transform.worldMatrix);

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale);
	collider_->SetOrientation(transform.rotate);

}

void Laser::Shot(const Vector3& position) {

	velocity_ = Vector3(position - transform.translate).Normalized();
	velocity_ *= speed_;
	isShot_ = true;
}

void Laser::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Block_Stay" ||
		collisionInfo.collider->GetName() == "Block_Shot") {

		isStop_ = true;

	}
	else {
		isStop_ = false;
	}

}
