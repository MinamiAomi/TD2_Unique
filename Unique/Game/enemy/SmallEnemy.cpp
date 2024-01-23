#include "SmallEnemy.h"
#include "Graphics/ResourceManager.h"
#include "SmallEnemyManager.h"
#include "Game/player/Player.h"

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

	if (coolTimer_ > 0) {

		velocity_ = player_->GetPosition() - transform.translate;
		velocity_ = velocity_.Normalized();
		velocity_ /= 3.0f;
		coolTimer_--;

		//規定フレームで移動に移行
		if (coolTimer_ <= 0) {
			moveTimer_ = kMaxMoveTime_;
		}

	}

	if (hp_ <= 0) {

		transform.translate += knockBackVelocity_;

		if (--deadCount_ <= 0) {
			isDead_ = true;
		}

	}
	else {

		//移動中の敵の場合
		if (collider_->GetName() == "Small_Enemy") {

			if (knockBackCount_ > 0) {

				transform.translate += knockBackVelocity_;

				knockBackVelocity_ /= 1.05f;

				if (knockBackVelocity_.Length() < 0.05f) {
					knockBackVelocity_ = Vector3::zero;
				}

				knockBackCount_--;

			}
			else {

				//移動更新
				if (moveTimer_ > 0) {

					transform.translate += velocity_;
					moveTimer_--;

					//規定フレームで一旦停止
					if (moveTimer_ <= 0) {
						coolTimer_ = kMaxCoolTime_;
					}

				}

				Vector3 diff = player_->GetPosition() - transform.translate;
				transform.rotate = Quaternion::MakeFromTwoVector(Vector3::unitZ, diff.Normalized());

			}

		}

	}

	transform.UpdateMatrix();

	collider_->SetCenter(transform.worldMatrix.GetTranslate());
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);

	model_->SetWorldMatrix(transform.worldMatrix);

}

void SmallEnemy::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Player") {

		//攻撃を受けた地点からノックバック
		knockBackVelocity_ = transform.worldMatrix.GetTranslate() - player_->GetPosition();

		knockBackVelocity_.y = 0.0f;

		knockBackVelocity_ = knockBackVelocity_.Normalized();

		knockBackCount_ = kKnockBackTime_ / 2;

	}

}

void SmallEnemy::Damage(uint32_t val, const Vector3& affectPosition) {

	hp_ -= val;

	if (hp_ < 0) {
		hp_ = 0;
	}

	//攻撃を受けた地点からノックバック
	knockBackVelocity_ = transform.worldMatrix.GetTranslate() - affectPosition;

	knockBackVelocity_.y = 0.0f;

	knockBackVelocity_ = knockBackVelocity_.Normalized() * 1.5f;

	knockBackCount_ = kKnockBackTime_;

}
