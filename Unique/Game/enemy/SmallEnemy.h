#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class Player;

class SmallEnemy : public GameObject
{
public:
	SmallEnemy();
	virtual ~SmallEnemy();

	virtual void Initialize(const Vector3& startPosition);

	virtual void Update();

	void SetPlayer(Player* player) { player_ = player; }

	bool GetIsDead() const { return isDead_; }

	bool GetIsAffectedGravity() const { return isAffectedGravity_; }

	BoxCollider* GetCollider() { return collider_.get(); }

	virtual void Damage(uint32_t val, const Vector3& affectPosition);

protected:

	virtual void OnCollision(const CollisionInfo& collisionInfo);

protected:

	Player* player_ = nullptr;

	std::shared_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

	uint32_t kMaxHp_ = 3;

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

class BarrierEnemy : public SmallEnemy
{
public:
	BarrierEnemy();

	void Initialize(const Vector3& startPosition) override;

	void Update() override;

protected:

	void OnCollision(const CollisionInfo& collisionInfo) override;

private:

	int32_t barrierHp_ = 3;

};

