#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class Weapon : public GameObject
{
public:
	Weapon();
	~Weapon();

	void Initialize();

	void Update();

	BoxCollider* GetCollider() { return collider_.get(); }

	Vector3 GetPosition() {
		return Vector3{
			transform.worldMatrix.m[3][0],
			transform.worldMatrix.m[3][1],
			transform.worldMatrix.m[3][2],
		};
	}

	std::shared_ptr<ModelInstance> GetModel() { return model_; }

	bool isHit_ = false;

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	std::shared_ptr<ModelInstance> model_;

	std::unique_ptr<BoxCollider> collider_;

};
