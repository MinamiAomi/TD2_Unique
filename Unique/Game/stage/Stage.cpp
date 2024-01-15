#include "Stage.h"
#include "Graphics/ResourceManager.h"
#include "Engine/input/Input.h"

Stage::Stage()
{
	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
	collider_ = std::make_unique<BoxCollider>();
}

Stage::~Stage()
{
}

void Stage::Initialize() {

	transform.translate = { 0.0f,-2.0f,0.0f };
	transform.scale = { 100.0f,1.0f, 100.0f };
	transform.rotate = Quaternion::identity;
	transform.UpdateMatrix();
	
	model_->SetIsActive(true);
	model_->SetWorldMatrix(transform.worldMatrix);
	model_->SetColor({ 0.0f,0.0f,0.0f });

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Stage");

}

void Stage::Update() {

	transform.UpdateMatrix();
	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale);
	model_->SetWorldMatrix(transform.worldMatrix);

}

void Stage::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Weapon") {

		

	}

}
