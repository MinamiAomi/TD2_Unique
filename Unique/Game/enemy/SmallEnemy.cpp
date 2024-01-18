#include "SmallEnemy.h"
#include "Graphics/ResourceManager.h"

SmallEnemy::SmallEnemy()
{

	model_ = std::make_shared<ModelInstance>();
	model_->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));

}

SmallEnemy::~SmallEnemy()
{
}

void SmallEnemy::Initialize() {

}

void SmallEnemy::Update() {

}
