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

	for (uint32_t i = 0; i < 4; i++) {
		borders_[i] = std::make_shared<ModelInstance>();
		borders_[i]->SetModel(ResourceManager::GetInstance()->FindModel("border"));
		borderTransforms_[i] = std::make_unique<Transform>();
		pillars_[i] = std::make_shared<ModelInstance>();
		pillars_[i]->SetModel(ResourceManager::GetInstance()->FindModel("Pillar"));
		pillarTransforms_[i] = std::make_unique<Transform>();
	}

	for (uint32_t i = 0; i < 10; i++) {
		objects_[i] = std::make_shared<ModelInstance>();
		switch (i % 4)
		{
		default:
		case 1:
			objects_[i]->SetModel(ResourceManager::GetInstance()->FindModel("Crystal"));
			break;
		case 2:
			objects_[i]->SetModel(ResourceManager::GetInstance()->FindModel("Crystal2"));
			break;
		case 3:
			objects_[i]->SetModel(ResourceManager::GetInstance()->FindModel("Torus"));
			break;
		case 0:
			objects_[i]->SetModel(ResourceManager::GetInstance()->FindModel("Cage"));
			break;
		}
		objectTransforms_[i] = std::make_unique<Transform>();
	}

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
	
	for (uint32_t i = 0; i < 4; i++) {
		borders_[i]->SetColor({ 1.0f,0.7f,0.7f });
		borderTransforms_[i]->rotate = Quaternion::MakeForYAxis(i * 1.57f);
	}

	borderTransforms_[0]->translate = { 0.0f,50.0f,200.0f };
	borderTransforms_[1]->translate = { 200.0f,50.0f,0.0f };
	borderTransforms_[2]->translate = { 0.0f,50.0f,-200.0f };
	borderTransforms_[3]->translate = { -200.0f,50.0f,0.0f };
	pillarTransforms_[0]->translate = { 200.0f,0.0f,200.0f };
	pillarTransforms_[1]->translate = { -200.0f,0.0f,200.0f };
	pillarTransforms_[2]->translate = { 200.0f,0.0f,-200.0f };
	pillarTransforms_[3]->translate = { -200.0f,0.0f,-200.0f };

	for (uint32_t i = 0; i < 4; i++) {
		borderTransforms_[i]->UpdateMatrix();
		borders_[i]->SetWorldMatrix(borderTransforms_[i]->worldMatrix);
		pillarTransforms_[i]->UpdateMatrix();
		pillars_[i]->SetWorldMatrix(pillarTransforms_[i]->worldMatrix);
	}

	skydomeTransform_->translate = Vector3::zero;
	skydomeTransform_->scale = Vector3::one;
	skydomeTransform_->rotate = Quaternion::identity;
	skydomeTransform_->UpdateMatrix();

	for (uint32_t i = 0; i < 10; i++) {
		objectTransforms_[i]->SetParent(skydomeTransform_.get());
		objectTransforms_[i]->rotate = Quaternion::MakeFromAngleAxis(i * 0.3f, Vector3{ 0.5f * i,0.5f - i,1.0f + i }.Normalized());
		objectTransforms_[i]->scale = Vector3::one * 50.0f;
	}

	objectTransforms_[0]->translate = { 300.0f,-100.0f,0.0f };
	objectTransforms_[1]->translate = { 300.0f,-70.0f,300.0f };
	objectTransforms_[2]->translate = { -300.0f,-40.0f,300.0f };
	objectTransforms_[3]->translate = { -300.0f,-10.0f,0.0f };
	objectTransforms_[4]->translate = { -300.0f,20.0f,-300.0f };
	objectTransforms_[5]->translate = { 0.0f,50.0f,-300.0f };
	objectTransforms_[6]->translate = { 300.0f,80.0f,-300.0f };
	objectTransforms_[7]->translate = { 300.0f,110.0f,0.0f };
	objectTransforms_[8]->translate = { 300.0f,140.0f,300.0f };
	objectTransforms_[9]->translate = { 0.0f,170.0f,300.0f };

	for (uint32_t i = 0; i < 10; i++) {
		objectTransforms_[i]->UpdateMatrix();
		objects_[i]->SetWorldMatrix(objectTransforms_[i]->worldMatrix);
	}

	model_->SetIsActive(false);
	model_->SetWorldMatrix(transform.worldMatrix);
	model_->SetColor({ 0.2f,0.2f,0.2f });

	collider_->SetCenter(transform.translate);
	//コライダーのサイズを二倍にすると、Cubeモデルの見た目と合致するので二倍にしている
	collider_->SetSize(transform.scale * 2.0f);
	collider_->SetOrientation(transform.rotate);
	collider_->SetName("Stage");
	collider_->SetGameObject(this);
	collider_->SetIsActive(false);

}

void Stage::Update() {

	transform.UpdateMatrix();
	skydomeTransform_->rotate = Quaternion::MakeForYAxis(0.001f) * skydomeTransform_->rotate;
	skydomeTransform_->UpdateMatrix();
	collider_->SetCenter(transform.translate);
	collider_->SetSize(transform.scale);
	model_->SetWorldMatrix(transform.worldMatrix);
	skydome_->SetWorldMatrix(skydomeTransform_->worldMatrix);

	for (uint32_t i = 0; i < 4; i++) {
		borderTransforms_[i]->UpdateMatrix();
		borders_[i]->SetWorldMatrix(borderTransforms_[i]->worldMatrix);
		pillarTransforms_[i]->UpdateMatrix();
		pillars_[i]->SetWorldMatrix(pillarTransforms_[i]->worldMatrix);
	}

	for (uint32_t i = 0; i < 10; i++) {
		objectTransforms_[i]->rotate = 
			Quaternion::MakeFromAngleAxis(i * 0.003f + 0.003f, Vector3{ 0.5f * i,0.5f - i,1.0f + i }.Normalized()) * objectTransforms_[i]->rotate;
		objectTransforms_[i]->UpdateMatrix();
		objects_[i]->SetWorldMatrix(objectTransforms_[i]->worldMatrix);
	}

}

void Stage::OnCollision(const CollisionInfo& collisionInfo) {

	if (collisionInfo.collider->GetName() == "Weapon") {

		

	}

}
