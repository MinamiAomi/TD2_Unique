#include "Gravity.h"
#include "Graphics/ResourceManager.h"
#include "Game/enemy/Enemy.h"
#include <unordered_map>
#include "Game/enemy/EnemyCoreManager.h"

Gravity::Gravity()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
}

Gravity::~Gravity()
{
}

void Gravity::Initialize() {

	SetName("Gravity");

	transform.translate = Vector3::zero;
	transform.scale = Vector3::one;
	transform.rotate = Quaternion::identity;

	collider_ = std::make_unique<BoxCollider>();
	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Gravity");
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetGameObject(this);

}

void Gravity::Update() {

	/*Vector3 posA = { model_->position_.x, 0.0f,model_->position_.z };
	Vector3 posB = { player_->GetLocalPosition().x, 0.0f,player_->GetLocalPosition().z };

	if (player_->GetIsBreak() && Length(posB - posA) < 20.0f) {
		isDead_ = true;
	}*/

	if (!isShot_) {

	}
	else {

		transform.translate += velocity_;
		transform.rotate = Quaternion::MakeForYAxis(0.1f) * transform.rotate;

		if (--shotLiveTimer_ <= 0) {
			isShot_ = false;
			collider_->SetIsActive(false);
		}

	}

	transform.UpdateMatrix();

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);

	model_->SetWorldMatrix(transform.worldMatrix);

}

void Gravity::Shot(const Vector3& velocity) {

	velocity_ = velocity;
	velocity_.Normalize();
	velocity_ = player_->transform.rotate * velocity_;
	velocity_ *= 3.0f;
	isShot_ = true;
	collider_->SetName("Gravity_Shot");
	collider_->SetIsActive(true);

}

void Gravity::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Enemy" ||
		collisionInfo.collider->GetName() == "Enemy_Core") {

		auto go = collisionInfo.collider->GetGameObject();
		std::shared_ptr<EnemyCore> enemy = EnemyCoreManager::GetInstance()->GetCore(go);

		enemy->Damage(3);

		collider_->SetIsActive(false);
		
	}
	else if (collisionInfo.collider->GetName() == "Enemy_Bullet") {

		model_->SetColor({ 1.0f,1.0f,0.0f });

	}

}
