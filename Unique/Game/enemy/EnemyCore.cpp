#include "EnemyCore.h"
#include "Graphics/ResourceManager.h"

EnemyCore::EnemyCore()
{

	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	collider_ = std::make_unique<BoxCollider>();

}

EnemyCore::~EnemyCore()
{
}

void EnemyCore::Initialize(const Transform& newTransform, uint32_t number) {

	SetName("Enemy_Core");

	transform = newTransform;
	transform.UpdateMatrix();
	number_ = number;
	hp_ = kMaxHp_;

	collider_->SetName("Enemy_Core");
	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetIsActive(true);
	model_->SetWorldMatrix(transform.worldMatrix);
	model_->SetColor({ 1.0f,1.0f,1.0f });

}

void EnemyCore::Update() {

	transform.UpdateMatrix();

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);

	model_->SetWorldMatrix(transform.worldMatrix);

	if (hitCoolTime_ > 0) {
		hitCoolTime_--;
	}

	if (hitCoolTime_ % 2 == 0) {
		model_->SetIsActive(true);
	}
	else {
		model_->SetIsActive(false);
	}

}

void EnemyCore::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Weapon") {

		if (hitCoolTime_ == 0) {

			if (hp_ > 0) {
				hp_--;

				//死んだら判定消失
				if (hp_ <= 0) {
					hp_ = 0;
					model_->SetColor({ 1.0f,0.0f,0.0f });
					collider_->SetIsActive(false);
				}

				hitCoolTime_ = invincibleTime_;
			}
			else {
				hitCoolTime_ = invincibleTime_;
			}

		}

	}
	else if (collisionInfo.collider->GetName() == "Block_Shot") {


		if (hitCoolTime_ == 0) {

			if (hp_ > 0) {

				hp_ -= 3;

				//死んだら判定消失
				if (hp_ <= 0) {
					hp_ = 0;
					model_->SetColor({ 1.0f,0.0f,0.0f });
					collider_->SetIsActive(false);
				}

				hitCoolTime_ = invincibleTime_;
			}
			else {
				hitCoolTime_ = invincibleTime_;
			}

		}

	}

}
