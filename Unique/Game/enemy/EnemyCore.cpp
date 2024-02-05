#include "EnemyCore.h"
#include "Graphics/ResourceManager.h"

EnemyCore::EnemyCore()
{

	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Boss"));
	/*barrierModel_ = std::make_shared<ModelInstance>();
	barrierModel_->SetModel(ResourceManager::GetInstance()->FindModel("Enemy_Barrier"));*/
	collider_ = std::make_unique<BoxCollider>();
	/*barrierCollider_ = std::make_unique<SphereCollider>();
	barrierTransform_ = std::make_unique<Transform>();*/

}

EnemyCore::~EnemyCore()
{
	model_;
}

void EnemyCore::Initialize(const Transform& newTransform, uint32_t number) {

	SetName("Enemy_Core");

	transform = newTransform;
	transform.UpdateMatrix();
	number_ = number;
	hp_ = kMaxHp_;

	startPosition_ = transform.translate;
	endPosition_ = transform.translate;

	/*barrierTransform_->SetParent(&transform);
	barrierTransform_->scale = { 3.0f,3.0f,3.0f };

	barrierCollider_->SetCenter(barrierTransform_->worldMatrix.GetTranslate());
	barrierCollider_->SetRadius(3.0f);
	barrierCollider_->SetName("BossBarrier");
	barrierCollider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollisionBarrier(collisionInfo); });
	barrierCollider_->SetCollisionAttribute(0xfffffffd);
	barrierCollider_->SetCollisionMask(0x00000002);
	barrierCollider_->SetIsActive(false);*/

	collider_->SetName("Enemy_Core");
	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale * 1.5f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetIsActive(true);
	collider_->SetGameObject(this);
	collider_->SetCollisionAttribute(0xfffffffd);
	collider_->SetCollisionMask(0x00000002);
	model_->SetIsActive(true);
	model_->SetWorldMatrix(transform.worldMatrix);
	model_->SetColor({ 0.5f,0.0f,0.5f });
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Boss"));

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

	if (barrierHp_ <= 0) {
		startPosition_ = { 0.0f,15.0f,0.0f };
		endPosition_ = { 0.0f,0.0f,0.0f };
	}
	else {
		startPosition_ = { 0.0f,0.0f,0.0f };
		endPosition_ = { 0.0f,15.0f,0.0f };
	}

	transform.translate = endPosition_;

	if (player_) {

		if (barrierHp_ > 0) {
			Vector3 diff = player_->GetPosition() - transform.translate;
			diff.y = 0.0f;
			if (diff.Normalized().z >= -0.99f) {
				transform.rotate = Quaternion::MakeFromTwoVector(Vector3::unitZ, diff.Normalized());
			}
			else {
				transform.rotate = Quaternion::MakeForYAxis(3.14f);
			}
		}
		else {



		}
		

	}


	transform.UpdateMatrix();
	/*barrierTransform_->UpdateMatrix();*/

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale * 1.5f);
	collider_->SetOrientation(transform.rotate);

	/*barrierCollider_->SetCenter(barrierTransform_->worldMatrix.GetTranslate());*/

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
	else if (collisionInfo.collider->GetName() == "Weapon") {

	}


}

//void EnemyCore::OnCollisionBarrier(const CollisionInfo& collisionInfo) {
//
//
//
//}

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

void EnemyCore::Stan() {

	transform.rotate = Quaternion::identity;

	stanTimer_ = maxStanTime_;

	lerpT_ = 0.0f;

	isStan_ = true;

}

void EnemyCore::Recover() {

	barrierHp_ = 3;

	lerpT_ = 0.0f;

	isStan_ = false;

}
