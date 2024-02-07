#include "EnemyCore.h"
#include "Graphics/ResourceManager.h"

EnemyCore::EnemyCore()
{

	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Boss"));
	/*barrierModel_ = std::make_shared<ModelInstance>();
	barrierModel_->SetModel(ResourceManager::GetInstance()->FindModel("Enemy_Barrier"));*/
	collider_ = std::make_unique<BoxCollider>();
	for (uint32_t i = 0; i < 3; i++) {
		shieldModels_[i] = std::make_shared<ModelInstance>();
		shieldModels_[i]->SetModel(ResourceManager::GetInstance()->FindModel("Shield"));
		shieldTransforms_[i] = std::make_unique<Transform>();
	}
	shieldRotateTransform_ = std::make_unique<Transform>();

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

	shieldRotateTransform_->translate = newTransform.translate;
	shieldRotateTransform_->scale = Vector3::one * 5.0f;
	shieldRotateTransform_->rotate = Quaternion::identity;
	shieldRotateTransform_->UpdateMatrix();

	for (uint32_t i = 0; i < 3; i++) {
		shieldTransforms_[i]->SetParent(shieldRotateTransform_.get());
		shieldTransforms_[i]->scale = Vector3::one * 1.0f;
		shieldTransforms_[i]->rotate = Quaternion::MakeForYAxis(i * Math::ToRadian * 120.0f);
	}

	shieldTransforms_[0]->translate = Vector3::unitZ * 2.0f;
	shieldTransforms_[1]->translate = { 2.0f,0.0f,-2.0f };
	shieldTransforms_[2]->translate = { -2.0f,0.0f,-2.0f };

	for (uint32_t i = 0; i < 3; i++) {
		shieldTransforms_[i]->UpdateMatrix();
		shieldModels_[i]->SetWorldMatrix(shieldTransforms_[i]->worldMatrix);
		shieldModels_[i]->SetColor({ 0.0f,1.0f,1.0f });
	}

	collider_->SetName("Enemy_Core");
	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetCallback([this](const CollisionInfo& collisionInfo) {OnCollision(collisionInfo); });
	collider_->SetIsActive(true);
	collider_->SetGameObject(this);
	collider_->SetCollisionAttribute(0xfffffffd);
	collider_->SetCollisionMask(0x00000002);
	model_->SetIsActive(true);
	model_->SetWorldMatrix(transform.worldMatrix);
	model_->SetColor({ 0.6f,0.0f,0.0f });
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

		if (hitCoolTime_ <= 0) {

			if (isStan_) {
				collider_->SetName("Enemy_Core_Stan");
			}
			else {
				collider_->SetName("Enemy_Core");
			}

		}

	}

	if (barrierHp_ <= 0) {
		startPosition_ = { 0.0f,30.0f,0.0f };
		endPosition_ = { 0.0f,15.0f,0.0f };
	}
	else {
		startPosition_ = { 0.0f,15.0f,0.0f };
		endPosition_ = { 0.0f,30.0f,0.0f };
	}

	transform.translate = Vector3::Slerp(lerpT_, startPosition_, endPosition_);
	shieldRotateTransform_->translate = Vector3::Slerp(lerpT_, startPosition_, endPosition_);

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

			if (stanTimer_ % 60 < 30) {
				transform.rotate = Quaternion::MakeForXAxis(0.02f) * transform.rotate;
			}
			else {
				transform.rotate = Quaternion::MakeForXAxis(-0.02f) * transform.rotate;
			}

			transform.rotate = Quaternion::MakeForYAxis(0.03f) * transform.rotate;

		}	

	}

	if (isStan_) {

		if (--stanTimer_ <= 0) {
			Recover();
		}

	}

	transform.UpdateMatrix();
	/*barrierTransform_->UpdateMatrix();*/

	shieldRotateTransform_->rotate = Quaternion::MakeForYAxis(0.02f) * shieldRotateTransform_->rotate;
	shieldRotateTransform_->UpdateMatrix();

	for (int32_t i = 0; i < 3; i++) {

		if (barrierHp_ > i && hp_ > 0) {
			shieldModels_[i]->SetIsActive(true);
		}
		else {
			shieldModels_[i]->SetIsActive(false);
		}

		shieldTransforms_[i]->UpdateMatrix();
		shieldModels_[i]->SetWorldMatrix(shieldTransforms_[i]->worldMatrix);
	}

	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale * 2.0f);
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

		//if (hitCoolTime_ == 0) {

		//	if (hp_ > 0) {
		//		hp_--;

		//		//死んだら判定消失
		//		if (hp_ <= 0) {
		//			hp_ = 0;
		//			model_->SetColor({ 1.0f,0.0f,0.0f });
		//			collider_->SetIsActive(false);
		//		}

		//		hitCoolTime_ = invincibleTime_;
		//	}
		//	else {
		//		hitCoolTime_ = invincibleTime_;
		//	}

		//	collider_->SetName("Enemy_Core_Damage");

		//}

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

		collider_->SetName("Enemy_Core_Damage");

	}

}

void EnemyCore::Stan() {

	transform.rotate = Quaternion::identity;

	stanTimer_ = maxStanTime_;

	lerpT_ = 0.0f;

	collider_->SetName("Enemy_Core_Stan");

	isStan_ = true;

}

void EnemyCore::Recover() {

	barrierHp_ = 3;

	lerpT_ = 0.0f;

	collider_->SetName("Enemy_Core");

	isStan_ = false;

}

void EnemyCore::BarrierDamage(int32_t val) {

	if (hitCoolTime_ == 0) {

		barrierHp_ -= val;

		hitCoolTime_ = invincibleTime_;

		if (barrierHp_ < 0) {
			barrierHp_ = 0;
		}

		if (barrierHp_ <= 0) {
			Stan();
		}

		collider_->SetName("Enemy_Core_Damage");

	}

}
