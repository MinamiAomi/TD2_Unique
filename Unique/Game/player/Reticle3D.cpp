#include "Reticle3D.h"
#include "Graphics/ResourceManager.h"

Reticle3D::Reticle3D()
{

	reticleTex_ = ResourceManager::GetInstance()->FindTexture("reticle");

	sprite_ = std::make_unique<Sprite>();
	sprite_->SetTexture(reticleTex_);

}

Reticle3D::~Reticle3D()
{
}

void Reticle3D::Initialize() {

	sprite_->SetPosition({ 640.0f,360.0f });
	sprite_->SetTexcoordRect({ 128.0f,128.0f }, { 128.0f,128.0f });
	sprite_->SetScale({ 64.0f,64.0f });

}

void Reticle3D::Update() {

	if (camera_.get()) {

		//レティクル
		{

			//カメラからの距離
			const float kDistance = 120.0f;
			//オフセット
			Vector3 offset = { 0.0f,0.0f,1.0f };
			offset = camera_->GetTransform()->worldMatrix.ApplyRotation(offset);
			offset = offset.Normalized() * kDistance;
			reticlePos_ = camera_->GetTransform()->translate + offset;

		}

		{

			Vector3 posReticle = reticlePos_;
			//ビューポート
			Matrix4x4 matViewport = Matrix4x4::MakeViewport(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);
			Matrix4x4 cameraMatrix = camera_->GetTransform()->worldMatrix;
			Matrix4x4 matView = cameraMatrix.Inverse();
			Matrix4x4 matProjection = Matrix4x4::MakePerspectiveProjection(45.0f * Math::ToRadian, float(1280.0f) / float(720.0f), 0.1f, 1000.0f);
			Matrix4x4 matViewProjectionViewport = matView * matProjection * matViewport;
			posReticle = matViewProjectionViewport.ApplyTransformWDivide(posReticle);
			sprite_->SetPosition({ posReticle.x, posReticle.y });

		}

	}

}
