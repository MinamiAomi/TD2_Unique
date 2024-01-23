#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class Laser : public GameObject
{
public:
	Laser();
	~Laser();

	void Initialize();

	void Update();

	void Shot(const Vector3& position);

	bool GetIsDead() const { return isDead_; }

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	std::shared_ptr<ModelInstance> model_;

	bool isDead_ = false;

	bool isShot_ = false;

	bool isStop_ = false;

	Vector3 velocity_{};

	float speed_ = 3.0f;

	//生存時間
	int32_t liveTime_;

	uint32_t maxLiveTime_ = 120;

	std::unique_ptr<BoxCollider> collider_;

};
