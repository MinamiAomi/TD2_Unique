#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"
#include <array>

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

	std::array<std::shared_ptr<ModelInstance>, 4> borders_;

	std::array<std::unique_ptr<Transform>, 4> borderTransforms_;

	std::array<std::shared_ptr<ModelInstance>, 4> pillars_;

	std::array<std::unique_ptr<Transform>, 4> pillarTransforms_;

	std::array<std::shared_ptr<ModelInstance>, 10> objects_;

	std::array<std::unique_ptr<Transform>, 10> objectTransforms_;

	std::shared_ptr<ModelInstance> skydome_;

	std::unique_ptr<BoxCollider> collider_;

	std::shared_ptr<Transform> skydomeTransform_;

};

