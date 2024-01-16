//#include "Reticle3D.h"
//#include "Graphics/ResourceManager.h"
//
//Reticle3D::Reticle3D()
//{
//}
//
//Reticle3D::~Reticle3D()
//{
//}
//
//void Reticle3D::Initialize() {
//
//}
//
//void Reticle3D::Update() {
//
//	//レティクル
//	{
//
//		//カメラからの距離
//		const float kDistance = 120.0f;
//		//オフセット
//		Vector3 offset = { 0.0f,0.0f,1.0f };
//		offset = TransformNormal(offset, camera_->GetTransform()->worldMatrix);
//		offset = Normalize(offset) * kDistance;
//		reticlePos_ = camera_->GetTransform()->translate + offset;
//		reticle3D_->position_ = reticlePos_;
//
//	}
//
//	{
//
//		Vector3 posReticle = { reticle3D_->matWorld_.m[3][0],reticle3D_->matWorld_.m[3][1],reticle3D_->matWorld_.m[3][2] };
//		//ビューポート
//		Matrix4x4 matViewport = MakeViewportMatrix(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);
//		Matrix4x4 cameraMatrix = camera_->GetTransform()->worldMatrix;
//		Matrix4x4 matView = cameraMatrix.Inverse();
//		Matrix4x4 matProjection = MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 1000.0f);
//		Matrix4x4 matViewProjectionViewport = matView * matProjection * matViewport;
//		posReticle = CoordTransform(posReticle, matViewProjectionViewport);
//		reticle_->position_ = { posReticle.x - 64.0f, posReticle.y - 64.0f };
//
//	}
//
//}
