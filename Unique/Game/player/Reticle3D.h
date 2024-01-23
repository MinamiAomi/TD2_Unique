#pragma once
#include "Graphics/Sprite.h"
#include "Collision/GameObject.h"
#include "Game/followCamera/FollowCamera.h"

class Reticle3D : public GameObject
{
public:
	Reticle3D();
	~Reticle3D();

	void Initialize();

	void Update();

	void SetCamera(std::shared_ptr<FollowCamera> camera) { camera_ = camera; }

	const Vector3& GetReticlePosition() { return reticlePos_; }

private:

	std::shared_ptr<FollowCamera> camera_;

	std::shared_ptr<Texture> reticleTex_;

	std::unique_ptr<Sprite> sprite_;

	Vector3 reticlePos_;

};


