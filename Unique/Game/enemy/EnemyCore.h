#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"
#include "Game/player/Player.h"
#include <array>

class EnemyCore : public GameObject
{
public:
	EnemyCore();
	~EnemyCore();

	void Initialize(const Transform& newTransform, uint32_t number);

	void Update();

	void Damage(uint32_t val);

	void BarrierDamage(int32_t val);

	const int32_t& GetHp() { return hp_; }

	void SetIsActiveModel(bool flag) { model_->SetIsActive(flag); }

	void SetPlayer(Player* player) { player_ = player; }

	bool GetIsStan() const { return isStan_; }

	bool GetIsDead()const { return isDead_; }

	void SetDeadMotion(const Vector3& position);

	//移動開始前のポジション
	Vector3 startPosition_;

	//移動開始後のポジション
	Vector3 endPosition_;

	Quaternion startRotate_;

	Quaternion endRotate_;

	float lerpT_ = 0.0f;

	float lerpValue_ = 0.05f;

private:

	void OnCollision(const CollisionInfo& collisionInfo);

	/*void OnCollisionBarrier(const CollisionInfo& collisionInfo);*/

	void Stan();

	void Recover();

private:

	Player* player_ = nullptr;

	std::shared_ptr<ModelInstance> model_;
	/*std::shared_ptr<ModelInstance> barrierModel_;*/

	std::array<std::shared_ptr<ModelInstance>, 3> shieldModels_;

	std::array<std::unique_ptr<Transform>, 3> shieldTransforms_;

	std::unique_ptr<Transform> shieldRotateTransform_;

	std::unique_ptr<BoxCollider> collider_;

	/*std::unique_ptr<SphereCollider> barrierCollider_;

	std::unique_ptr<Transform> barrierTransform_;*/

	uint32_t number_;

	//速度
	Vector3 velocity_{};

	uint32_t kMaxHp_ = 20;

	int32_t hp_ = kMaxHp_;

	uint32_t invincibleTime_ = 30;

	int32_t hitCoolTime_ = 0;

	bool isActiveBarrier_ = true;

	int32_t maxBarrierHp_ = 3;

	int32_t barrierHp_ = maxBarrierHp_;

	int32_t maxStanTime_ = 500;

	int32_t stanTimer_ = 0;

	bool isStan_ = false;

	bool isStartKnockBack_ = false;

	bool isDead_ = false;

	Vector3 knockBackVelocity_{};

	size_t hitHeavySE_;
	size_t deathSE_;

};
