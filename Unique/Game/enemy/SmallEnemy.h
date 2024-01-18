#pragma once
#include "Graphics/Model.h"
#include "Collision/Collider.h"
#include "Collision/GameObject.h"

class SmallEnemy : public GameObject
{
public:
	SmallEnemy();
	~SmallEnemy();

	void Initialize();

	void Update();

private:

	std::shared_ptr<ModelInstance> model_;



};


