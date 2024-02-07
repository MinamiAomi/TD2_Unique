#include "BarrierBullet.h"
#include "Graphics/ResourceManager.h"
#include "Math/Random.h"
#include "BarrierBulletManager.h"
#include "Audio/Audio.h"

static Random::RandomNumberGenerator randomNumberGenerator;

BarrierBullet::BarrierBullet()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("BarrierBullet"));
	barrierModel_ = std::make_shared<ModelInstance>();
	barrierModel_->SetModel(ResourceManager::GetInstance()->FindModel("Enemy_Barrier"));
	barrierScaleTransform_ = std::make_unique<Transform>();
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

	barrierScaleTransform_->SetParent(&transform);
	barrierScaleTransform_->translate = Vector3::zero;
	barrierScaleTransform_->rotate = Quaternion::identity;
	barrierScaleTransform_->scale = Vector3::one * 1.3f;
	barrierScaleTransform_->UpdateMatrix();

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetName("Barrier_Bullet");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetGameObject(this);
	collider_->SetCollisionAttribute(0xfffffffd);
	collider_->SetCollisionMask(0x00000002);

	Vector3 EulerRotate = { randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f,
		randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f, randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f };

	transform.rotate = Quaternion::MakeFromEulerAngle(EulerRotate) * transform.rotate;
	collider_->SetOrientation(transform.rotate);
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);
	model_->SetColor({ 1.0f,0.0f,0.0f });
	barrierModel_->SetWorldMatrix(transform.worldMatrix);

	barrierBreakSE_ = Audio::GetInstance()->SoundLoadWave("./Resources/sound/barrierBreak.wav");

}

void BarrierBullet::Update() {

	if (transform.scale.x < 3.0f) {

		transform.scale.x += 0.3f;
		transform.scale.y += 0.3f;
		transform.scale.z += 0.3f;

	}

	if (isShot_ && !isDead_ && velocity_.Length() >= 0.1f && collider_->GetName() == "Barrier_Bullet") {

		transform.translate += velocity_;

		velocity_ /= 1.01f;

		transform.rotate = Quaternion::MakeForZAxis(velocity_.Length()) * transform.rotate;

		if (velocity_.Length() < 0.1f) {
			velocity_ = Vector3::zero;
		}

	}
	//バリア状態でしばらく放置されたら消える
	else if(isShot_ && isActiveBarrier_){

		if (--liveTime_ <= 0) {
			isDead_ = true;
		}

	}
	//バリアが破壊されてからしばらく放置で消える
	else if (!isActiveBarrier_) {

		if (--barrierBreakLiveTime_ <= 0) {
			isDead_ = true;
		}

	}

	transform.UpdateMatrix();
	barrierScaleTransform_->UpdateMatrix();

	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	model_->SetWorldMatrix(transform.worldMatrix);
	barrierModel_->SetWorldMatrix(barrierScaleTransform_->worldMatrix);

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

void BarrierBullet::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Player" &&
		collider_->GetName() == "Barrier_Bullet") {

		isDead_ = true;

	}
	else if (collisionInfo.collider->GetName() == "Weapon" ||
		collisionInfo.collider->GetName() == "Gravity_Attack") {

		if (isActiveBarrier_) {
			Audio::GetInstance()->SoundPlayWave(barrierBreakSE_);

			isActiveBarrier_ = false;
		}

	}

}
