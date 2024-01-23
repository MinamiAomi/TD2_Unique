#pragma once
#include "Graphics/Model.h"
#include "Game/player/Player.h"
#include <memory>
#include "Collision/Collider.h"
#include "Math/Camera.h"
#include "Collision/GameObject.h"

class EnemyCore;

class Block : public GameObject
{
public:
	Block();
	~Block();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="pos">配置場所</param>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="blockSize">大きさ(整数で)</param>
	void Initialize(const Vector3& pos, Player* player, const Vector3& blockSize);

	void Update();

	Vector3 GetPosition() {
		return Vector3{
			transform.worldMatrix.m[3][0],
			transform.worldMatrix.m[3][1],
			transform.worldMatrix.m[3][2]
		};
	}

	bool GetIsDead() { return isDead_; }

	void Shot(const Vector3& velocity);

	void SetIsDead(bool flag) { isDead_ = flag; }

	bool GetIsShot() const { return isShot_; }

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	std::shared_ptr<ModelInstance> model_;

	bool isBreak_ = false;

	bool isDead_ = false;

	Player* player_ = nullptr;

	//ブロックの大きさ
	Vector3 blockSize_{};

	uint32_t sizeX_;
	uint32_t sizeY_;
	uint32_t sizeZ_;

	uint32_t particleCount_;

	Vector3 velocity_{};

	//生存時間
	int32_t liveTimer_ = 360;

	//打たれたかどうか
	bool isShot_ = false;

	//打てる状態(強化された状態)かどうか
	bool canShot_ = false;

	//発射後の生存時間
	int32_t shotLiveTimer_ = 180;

	//当たり判定(aabb)
	std::unique_ptr<BoxCollider> collider_;

};

