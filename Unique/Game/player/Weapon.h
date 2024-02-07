#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"
#include "Audio/Audio.h"

class Player;

class Weapon : public GameObject
{
public:
	Weapon();
	~Weapon();

	enum GravityLevel {
		kSmall = 1, //小範囲
		kMedium, //中範囲
		kWide, //広範囲
	};

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

	const GravityLevel& GetLevel() const { return gravityLevel_; }

	bool isThrust_ = false;

	bool isAttack_ = false;

	//衝撃波発動中か
	bool isShockWave_ = false;
	
	std::shared_ptr<Transform> modelBodyTransform_;

	//武器の初期SRTへ戻す
	void SetDefault();

	void SetIsAttack(bool isAttack) {
		isAttack_ = isAttack;
		collider_->SetIsActive(isAttack);
	}

	//ウェポンの衝撃波当たり判定。三撃目の最後に発動
	std::unique_ptr<SphereCollider> shockWaveCollider_;

private:

	void OnCollision(const CollisionInfo& collisionInfo);

	void GravityOnCollision(const CollisionInfo& collisionInfo);

	void ShockWaveCollision(const CollisionInfo& collisionInfo);

	void Break();

	void Reset();

private:

	Audio* audio_ = nullptr;

	Player* player_ = nullptr;

	std::shared_ptr<Transform> gravityTransform_;
	std::shared_ptr<Transform> gravityScaleTransform_;

	Vector3 velocity_{};

	std::shared_ptr<ModelInstance> model_;

	std::shared_ptr<ModelInstance> modelBody_;

	std::shared_ptr<ModelInstance> gravityModel_;

	std::unique_ptr<BoxCollider> collider_;

	//武器とプレイヤーの間を攻撃判定にするためのコライダー
	std::unique_ptr<SphereCollider> spaceCollider_;

	std::unique_ptr<SphereCollider> gravityCollider_;

	//武器とプレイヤーの間を攻撃判定にするためのコライダー
	std::unique_ptr<SphereCollider> gravitySpaceCollider_;

	uint32_t energyCount_;

	//重力の強さ
	GravityLevel gravityLevel_;

	//重力による振りの遅延量
	int32_t gravityDelay_ = 5;

	//重力を付与しているかどうか
	bool isGravity_ = false;

	bool isShot_ = false;

	uint32_t kMaxShotTime_ = 30;

	int32_t shotTimer_ = kMaxShotTime_;

	bool isBreak_ = false;

	uint32_t kMaxBreakTime_ = 30;

	int32_t breakTimer_ = kMaxBreakTime_;

	int32_t barrierBulletCount_ = 0;

	size_t hitRightSE_;
	size_t hitHeavySE_;
	size_t gravityShotSE_;
	size_t gravityBreakSE_;
	size_t gravitySE_;
	size_t gravitySEHandle_ = UNUSED_PLAY_HANDLE;

};
