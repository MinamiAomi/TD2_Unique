#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class EnemyCore : public GameObject
{
public:
	EnemyCore();
	~EnemyCore();

	void Initialize(const Transform& newTransform, uint32_t number);

	void Update();

	void Damage(uint32_t val);

	const int32_t& GetHp() { return hp_; }

	void SetIsActiveModel(bool flag) { model_->SetIsActive(flag); }

	//移動開始前のポジション
	Vector3 startPosition_;

	//移動開始後のポジション
	Vector3 endPosition_;

	float lerpT_ = 0.0f;

	float lerpValue_ = 0.05f;

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	std::shared_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	uint32_t number_;

	//速度
	Vector3 velocity_{};

	uint32_t kMaxHp_ = 3;

	int32_t hp_ = kMaxHp_;

	uint32_t invincibleTime_ = 30;

	int32_t hitCoolTime_ = 0;

};
