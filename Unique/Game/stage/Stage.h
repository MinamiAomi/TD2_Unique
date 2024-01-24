#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class Stage : public GameObject
{
public:
	Stage();
	~Stage();

	void Initialize();

	void Update();

	BoxCollider* GetCollision() { return collider_.get(); }

private:

	void OnCollision(const CollisionInfo& collisionInfo);

private:

	std::shared_ptr<ModelInstance> model_;

	std::shared_ptr<ModelInstance> skydome_;

	std::unique_ptr<BoxCollider> collider_;

	std::shared_ptr<Transform> skydomeTransform_;

};

