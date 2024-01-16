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

private:

	std::shared_ptr<Model> model_;

	std::unique_ptr<BoxCollider> collider_;


};
