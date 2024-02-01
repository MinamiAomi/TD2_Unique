#pragma once
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"
#include "Collision/GameObject.h"
#include "Math/Camera.h"

class Effect : public GameObject
{
public:
	Effect();
	~Effect();

	void Initialize();

	void Update();

	void SetIsActive(bool flag) { model_->SetIsActive(flag); }

	void SetCamera(const std::shared_ptr<Camera>& camera) { camera_ = camera; }

	void SetTexture(const std::shared_ptr<Texture>& texture){}

private:

	std::shared_ptr<Camera> camera_;

	std::shared_ptr<ModelInstance> model_;

	Vector3 velocity_{};

	Matrix4x4 billBoardMatrix_;

};


