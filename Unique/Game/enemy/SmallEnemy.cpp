#include "SmallEnemy.h"
#include "Graphics/ResourceManager.h"
#include "SmallEnemyManager.h"
#include "Game/player/Player.h"

///----------------------------------------------------------------
/// 通常の雑魚敵
///----------------------------------------------------------------

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
	transform.scale = Vector3::one * 2.0f;
	transform.rotate = Quaternion::identity;

	model_->SetColor({ 1.0f,0.0f,0.0f });

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
		if (collider_->GetName() == "Small_Enemy" ||
			collider_->GetName() == "Small_Enemy_Damaged") {

			if (knockBackCount_ > 0) {

				transform.translate += knockBackVelocity_;

				knockBackVelocity_ /= 1.05f;

				if (knockBackVelocity_.Length() < 0.05f) {
					knockBackVelocity_ = Vector3::zero;
				}

				if (--knockBackCount_ <= 0) {
					collider_->SetName("Small_Enemy");
				}

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

		//プレイヤーと当たった地点からノックバック
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

	collider_->SetName("Small_Enemy_Damaged");

	//攻撃を受けた地点からノックバック
	knockBackVelocity_ = transform.worldMatrix.GetTranslate() - affectPosition;

	knockBackVelocity_.y = 0.0f;

	knockBackVelocity_ = knockBackVelocity_.Normalized() * (1.0f + float(val / 2.0f));

	knockBackCount_ = kKnockBackTime_;

}

///----------------------------------------------------------------
/// バリア持ちの敵
///----------------------------------------------------------------

BarrierEnemy::BarrierEnemy()
{

	/*barrierModel_ = std::make_shared<ModelInstance>();
	barrierModel_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));*/

}

void BarrierEnemy::Initialize(const Vector3& startPosition) {

	SmallEnemy::Initialize(startPosition);

	model_->SetColor({ 0.0f,1.0f,1.0f });

	SetName("Barrier_Enemy");

	collider_->SetName("Barrier_Enemy");

}

void BarrierEnemy::Update() {

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
		if (collider_->GetName() == "Barrier_Enemy" ||
			collider_->GetName() == "Small_Enemy" ||
			collider_->GetName() == "Barrier_Enemy_Damaged") {

			if (knockBackCount_ > 0) {

				transform.translate += knockBackVelocity_;

				knockBackVelocity_ /= 1.05f;

				if (knockBackVelocity_.Length() < 0.05f) {
					knockBackVelocity_ = Vector3::zero;
				}

				if (--knockBackCount_ <= 0) {

					if (barrierHp_ > 0) {
						collider_->SetName("Barrier_Enemy");
					}
					else {
						collider_->SetName("Small_Enemy");
					}

				}

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

void BarrierEnemy::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Player") {

		//プレイヤーと当たった地点からノックバック
		knockBackVelocity_ = transform.worldMatrix.GetTranslate() - player_->GetPosition();

		knockBackVelocity_.y = 0.0f;

		knockBackVelocity_ = knockBackVelocity_.Normalized();

		knockBackCount_ = kKnockBackTime_ / 2;

	}

}

void BarrierEnemy::Damage(uint32_t val, const Vector3& affectPosition) {

	//最終ダメージ
	int32_t tmpDamage = val;

	//バリアがダメージと同等以上の場合
	if (barrierHp_ >= tmpDamage) {

		//バリアにダメージの値をそのまま与える
		barrierHp_ -= tmpDamage;
		//ダメージの値を0にする
		tmpDamage = 0;

	}
	//バリアがダメージより低い場合
	else if (barrierHp_ < tmpDamage) {

		//ダメージをバリアの値分減らす
		tmpDamage -= barrierHp_;
		//バリアのライフを0にする
		barrierHp_ = 0;

	}

	if (barrierHp_ <= 0) {
		model_->SetColor({ 1.0f,0.0f,0.0f });
	}

	//最終的なダメージを本体に与える
	hp_ -= tmpDamage;

	//0以下になった場合0とする
	if (hp_ < 0) {
		hp_ = 0;
	}

	collider_->SetName("Barrier_Enemy_Damaged");

	//攻撃を受けた地点からノックバック
	knockBackVelocity_ = transform.worldMatrix.GetTranslate() - affectPosition;

	knockBackVelocity_.y = 0.0f;

	knockBackVelocity_ = knockBackVelocity_.Normalized() * (1.0f + float(val / 2.0f));

	knockBackCount_ = kKnockBackTime_;

}
