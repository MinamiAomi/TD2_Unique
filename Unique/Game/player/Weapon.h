#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class Player;

class Weapon : public GameObject
{
public:
	Weapon();
	~Weapon();

	void Initialize();

	void Update();

	BoxCollider* GetCollider() { return collider_.get(); }

	SphereCollider* GetGravityCollider() { return gravityCollider_.get(); }

	Vector3 GetPosition() {
		return Vector3{
			transform.worldMatrix.m[3][0],
			transform.worldMatrix.m[3][1],
			transform.worldMatrix.m[3][2],
		};
	}

	std::shared_ptr<ModelInstance> GetModel() { return model_; }

	void SetPlayer(Player* player) { player_ = player; }

	void Shot(const Vector3& velocity);

	bool GetIsShot() const { return isShot_; }

	bool isHit_ = false;

	bool GetIsGravity() const { return isGravity_; }

	//重力付与
	void AddGravity();

	//遅延量取得
	const int32_t& GetDelay() { return gravityDelay_; }

	bool isThrust_ = false;

private:

	void OnCollision(const CollisionInfo& collisionInfo);

	void GravityOnCollision(const CollisionInfo& collisionInfo);

	void GravityDamageOnCollision(const CollisionInfo& collisionInfo);

	void Break();

	void Reset();

private:

	enum GravityLevel {
		kSmall = 1, //小範囲
		kMedium, //中範囲
		kWide, //広範囲
	};

	Player* player_ = nullptr;

	std::shared_ptr<Transform> gravityTransform_;

	Vector3 velocity_{};

	std::shared_ptr<ModelInstance> model_;

	std::shared_ptr<ModelInstance> gravityModel_;

	std::unique_ptr<BoxCollider> collider_;

	std::unique_ptr<SphereCollider> gravityCollider_;

	uint32_t energyCount_;

	//重力の強さ
	GravityLevel gravityLevel_;

	//重力による振りの遅延量
	int32_t gravityDelay_ = 5;

	//重力を付与しているかどうか
	bool isGravity_ = false;

	bool isShot_ = false;

	uint32_t kMaxShotTime_ = 60;

	int32_t shotTimer_ = kMaxShotTime_;

	bool isBreak_ = false;

	uint32_t kMaxBreakTime_ = 30;

	int32_t breakTimer_ = kMaxBreakTime_;

};
