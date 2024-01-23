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

	startPosition_ = transform.translate;
	endPosition_ = transform.translate;

	collider_->SetName("Enemy_Core");
	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetIsActive(true);
	collider_->SetGameObject(this);
	model_->SetWorldMatrix(transform.worldMatrix);
	model_->SetColor({ 1.0f,1.0f,1.0f });

}

void EnemyCore::Update() {

	//線形補間の媒介変数更新
	if (lerpT_ < 1.0f) {
		
		lerpT_ += lerpValue_;

		if (lerpT_ > 1.0f) {
			lerpT_ = 1.0f;
		}

	}

	if (hitCoolTime_ > 0) {
		hitCoolTime_--;
	}

	transform.translate = Vector3::Slerp(lerpT_, startPosition_, endPosition_);

	Vector3 rotateSpeed = { 0.01f + float(hitCoolTime_ * 0.005f),0.01f + float(hitCoolTime_ * 0.005f),
		0.01f + float(hitCoolTime_ * 0.005f) };
	transform.rotate = Quaternion::MakeFromEulerAngle(rotateSpeed) * transform.rotate;

	transform.UpdateMatrix();

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);

	model_->SetWorldMatrix(transform.worldMatrix);

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
	//else if (collisionInfo.collider->GetName() == "Block_Shot") {


	//	if (hitCoolTime_ == 0) {

	//		if (hp_ > 0) {

	//			hp_ -= 3;

	//			//死んだら判定消失
	//			if (hp_ <= 0) {
	//				hp_ = 0;
	//				model_->SetColor({ 1.0f,0.0f,0.0f });
	//				collider_->SetIsActive(false);
	//			}

	//			hitCoolTime_ = invincibleTime_;
	//		}
	//		else {
	//			hitCoolTime_ = invincibleTime_;
	//		}

	//	}

	//}

}

void EnemyCore::Damage(uint32_t damage) {

	if (hitCoolTime_ == 0) {

		if (hp_ > 0) {

			hp_ -= damage;

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
