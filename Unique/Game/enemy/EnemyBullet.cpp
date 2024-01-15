#include "EnemyBullet.h"
#include "Graphics/ResourceManager.h"
#include "Math/Random.h"

static Random::RandomNumberGenerator randomNumberGenerator;

EnemyBullet::EnemyBullet()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	collider_ = std::make_unique<BoxCollider>();

}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Initialize(const Vector3& position) {

	SetName("Enemy_Bullet");

	transform.scale = Vector3::zero;
	transform.translate = position;
	transform.rotate = Quaternion::identity;

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetName("Enemy_Bullet");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });

	Vector3 EulerRotate = { randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f,
		randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f, randomNumberGenerator.NextFloatRange(0.0f,300.0f) / 100.0f };

	transform.rotate = Quaternion::MakeFromEulerAngle(EulerRotate) * transform.rotate;
	collider_->SetOrientation(transform.rotate);
	transform.UpdateMatrix();
	model_->SetWorldMatrix(transform.worldMatrix);

}

void EnemyBullet::Update() {

	if (transform.scale.x < 1.0f) {

		transform.scale.x += 0.1f;
		transform.scale.y += 0.1f;
		transform.scale.z += 0.1f;

	}

	if (isShot_ && !isDead_) {
		transform.translate += velocity_;

		liveTime_--;

		if (liveTime_ <= 0) {
			isDead_ = true;
		}

	}
	else {

	}

	transform.UpdateMatrix();

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);

	model_->SetWorldMatrix(transform.worldMatrix);

}

void EnemyBullet::Shot(const Vector3& position) {

	liveTime_ = maxLiveTime_;
	velocity_ = Vector3(position - transform.translate).Normalize();
	velocity_ *= 3.0f;
	isShot_ = true;

}

void EnemyBullet::Charge(const Vector3& position) {

	position;

}

void EnemyBullet::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Player" ||
		collisionInfo.collider->GetName() == "Block_Stay" ||
		collisionInfo.collider->GetName() == "Block_Shot") {

		isDead_ = true;

	}

}
