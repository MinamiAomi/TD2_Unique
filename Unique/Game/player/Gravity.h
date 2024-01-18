#pragma once
#include "Graphics/Model.h"
#include "Game/player/Player.h"
#include <memory>
#include "Collision/Collider.h"
#include "Math/Camera.h"
#include "Collision/GameObject.h"

class EnemyCore;

class Player;

class Gravity : public GameObject
{
public:
	Gravity();
	~Gravity();

	void Initialize();

	void Update();

	Vector3 GetPosition() {
		return Vector3{
			transform.worldMatrix.m[3][0],
			transform.worldMatrix.m[3][1],
			transform.worldMatrix.m[3][2]
		};
	}

	void Shot(const Vector3& velocity);

	bool GetIsShot() const { return isShot_; }

	void SetPlayer(Player* player) { player_ = player; }

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	std::shared_ptr<ModelInstance> model_;

	Player* player_ = nullptr;

	Vector3 velocity_{};

	//打たれたかどうか
	bool isShot_ = false;

	//打てる状態(強化された状態)かどうか
	bool canShot_ = false;

	//発射後の生存時間
	int32_t shotLiveTimer_ = 180;

	//当たり判定(aabb)
	std::unique_ptr<BoxCollider> collider_;

};

