#pragma once
#include "Graphics/Sprite.h"
#include "Collision/GameObject.h"
#include "Game/followCamera/FollowCamera.h"
#include "Graphics/Model.h"

class Reticle3D : public GameObject
{
public:
	Reticle3D();
	~Reticle3D();

	void Initialize();

	void Update();

	void SetCamera(std::shared_ptr<FollowCamera> camera) { camera_ = camera; }

	const Vector3& GetReticlePosition() { return reticlePos_; }

	void SetIsActive(bool flag) { sprite_->SetIsActive(flag); }

private:

	std::shared_ptr<FollowCamera> camera_;

	std::shared_ptr<Texture> reticleTex_;

	std::unique_ptr<Sprite> sprite_;

	std::shared_ptr<ModelInstance> model_;

	Vector3 reticlePos_;

};


