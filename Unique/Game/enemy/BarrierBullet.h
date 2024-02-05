#pragma once
#include "Graphics/Model.h"
#include <memory>
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class BarrierBullet : public GameObject
{
public:
	BarrierBullet();
	~BarrierBullet();

	void Initialize(const Vector3& position);

	void Update();

	bool GetIsDead() const { return isDead_; }

	void SetIsDead(bool flag) { isDead_ = flag; }

	bool GetIsShot() const { return isShot_; }

	bool GetIsBarrier()const { return isActiveBarrier_; }

	void Shot(const Vector3& position);

	void SetPosition(const Vector3& position) { transform.translate = position; }

	void Charge(const Vector3& position);

	void SetIsActive(bool flag) {
		model_->SetIsActive(flag);
		collider_->SetIsActive(flag);
	}

	BoxCollider* GetCollider() { return collider_.get(); }

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	std::shared_ptr<ModelInstance> model_;

	std::shared_ptr<ModelInstance> barrierModel_;

	bool isDead_ = false;

	bool isShot_ = false;

	Vector3 velocity_{};

	//生存時間
	int32_t liveTime_;

	uint32_t maxLiveTime_ = 120;

	//当たり判定
	std::unique_ptr<BoxCollider> collider_;

	bool isActiveBarrier_ = true;

};
