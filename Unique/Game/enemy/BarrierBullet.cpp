#include "BarrierBullet.h"
#include "Graphics/ResourceManager.h"
#include "Math/Random.h"
#include "BarrierBulletManager.h"

static Random::RandomNumberGenerator randomNumberGenerator;

BarrierBullet::BarrierBullet()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("BarrierBullet"));
	barrierModel_ = std::make_shared<ModelInstance>();
	barrierModel_->SetModel(ResourceManager::GetInstance()->FindModel("Enemy_Barrier"));
	collider_ = std::make_unique<BoxCollider>();
	
}

BarrierBullet::~BarrierBullet()
{
	
}

void BarrierBullet::Initialize(const Vector3& position) {

	SetName("Barrier_Bullet");

	transform.scale = Vector3::zero;
	transform.translate = position;
	transform.rotate = Quaternion::identity;

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetName("Barrier_Bullet");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetGameObject(this);
	collider_->SetCollisionAttribute(0xfffffffc);
	collider_->SetCollisionMask(0x00000003);

	Vector3 EulerRotate = { randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f,
		randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f, randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f };

	transform.rotate = Quaternion::MakeFromEulerAngle(EulerRotate) * transform.rotate;
	collider_->SetOrientation(transform.rotate);
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
	barrierModel_->SetWorldMatrix(transform.worldMatrix);

}

void BarrierBullet::Update() {

	if (transform.scale.x < 3.0f) {

		transform.scale.x += 0.3f;
		transform.scale.y += 0.3f;
		transform.scale.z += 0.3f;

	}

	if (isShot_ && !isDead_ && velocity_.Length() >= 0.1f && collider_->GetName() == "Barrier_Bullet") {

		transform.translate += velocity_;

		velocity_ /= 1.05f;

		transform.rotate = Quaternion::MakeForZAxis(velocity_.Length()) * transform.rotate;

		if (velocity_.Length() < 0.1f) {
			velocity_ = Vector3::zero;
		}

	}
	else {

	}

	transform.UpdateMatrix();

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
	barrierModel_->SetWorldMatrix(transform.worldMatrix);

	if (isActiveBarrier_) {
		barrierModel_->SetIsActive(true);
	}
	else {
		barrierModel_->SetIsActive(false);
	}

}

void BarrierBullet::Shot(const Vector3& position) {

	liveTime_ = maxLiveTime_;
	velocity_ = Vector3(position - transform.translate).Normalized();
	velocity_ *= 1.0f;
	isShot_ = true;

}

void BarrierBullet::Charge(const Vector3& position) {

	position;

}

void BarrierBullet::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Player") {

		isDead_ = true;

	}
	else if (collisionInfo.collider->GetName() == "Weapon" ||
		collisionInfo.collider->GetName() == "Gravity_Attack") {

		isShot_ = false;
		isActiveBarrier_ = false;

	}

}
