#include "Stage.h"
#include "Graphics/ResourceManager.h"
#include "Engine/input/Input.h"

Stage::Stage()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	skydome_ = std::make_shared<ModelInstance>();
	skydome_->SetModel(ResourceManager::GetInstance()->FindModel("Skydome"));
	skydome_->SetCastShadow(false);
	skydome_->SetReciveShadow(false);
	skydome_->SetReflection(false);
	skydome_->SetUseLighting(false);
	collider_ = std::make_unique<BoxCollider>();
	skydomeTransform_ = std::make_shared<Transform>();
}

Stage::~Stage()
{
}

void Stage::Initialize() {

	transform.translate = { 0.0f,-2.0f,0.0f };
	transform.scale = { 500.0f,1.0f, 500.0f };
	transform.rotate = Quaternion::identity;
	transform.UpdateMatrix();
	
	skydomeTransform_->translate = Vector3::zero;
	skydomeTransform_->scale = Vector3::one;
	skydomeTransform_->rotate = Quaternion::identity;
	skydomeTransform_->UpdateMatrix();

	/*model_->SetIsActive(false);*/
	model_->SetWorldMatrix(transform.worldMatrix);
	model_->SetColor({ 0.2f,0.2f,0.2f });

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Stage");
	collider_->SetGameObject(this);

}

void Stage::Update() {

	transform.UpdateMatrix();
	skydomeTransform_->UpdateMatrix();
	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale);
	model_->SetWorldMatrix(transform.worldMatrix);
	skydome_->SetWorldMatrix(skydomeTransform_->worldMatrix);

}

void Stage::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Weapon") {

		

	}

}
