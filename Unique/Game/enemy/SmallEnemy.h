#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class Player;

class SmallEnemy : public GameObject
{
public:
	SmallEnemy();
	~SmallEnemy();

	void Initialize(const Vector3& startPosition);

	void Update();

	void SetPlayer(Player* player) { player_ = player; }

	bool GetIsDead() const { return isDead_; }

	bool GetIsAffectedGravity() const { return isAffectedGravity_; }

	BoxCollider* GetCollider() { return collider_.get(); }

	void Damage(uint32_t val, const Vector3& affectPosition);

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	Player* player_ = nullptr;

	std::shared_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	uint32_t kMaxHp_ = 1;

	int32_t hp_ = kMaxHp_;

	bool isDead_ = false;

	bool isAffectedGravity_ = false;

	Vector3 knockBackVelocity_{};

	Vector3 velocity_{};

	//死亡までのフレーム
	int32_t deadCount_ = 10;

	//ノックバック中のタイムカウント
	uint32_t kKnockBackTime_ = 30;

	int32_t knockBackCount_ = 0;

	uint32_t kMaxMoveTime_ = 30;

	int32_t moveTimer_ = 0;

	uint32_t kMaxCoolTime_ = 30;

	int32_t coolTimer_ = kMaxCoolTime_;

};


