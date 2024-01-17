#include "Unique.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Model.h"
#include "Graphics/ResourceManager.h"

#include "GameScene.h"

void Unique::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();

	auto resourceManager = ResourceManager::GetInstance();
	resourceManager->AddModel("Cube", Model::Load("./Resources/cube/cube.obj"));
	std::shared_ptr<Texture> playerHpTex = std::make_shared<Texture>();
	playerHpTex->Load("./Resources/UI/p_life.png");
	resourceManager->AddTexture("player_hp", playerHpTex);
	std::shared_ptr<Texture> enemyHpTex = std::make_shared<Texture>();
	enemyHpTex->Load("./Resources/UI/e_life.png");
	resourceManager->AddTexture("enemy_hp", enemyHpTex);
	std::shared_ptr<Texture> reticle = std::make_shared<Texture>();
	reticle->Load("./Resources/UI/reticle.png");
	resourceManager->AddTexture("reticle", reticle);
	std::shared_ptr<Texture> UI_A = std::make_shared<Texture>();
	UI_A->Load("./Resources/UI/ui_1.png");
	resourceManager->AddTexture("UI_A", UI_A);
	std::shared_ptr<Texture> UI_LB = std::make_shared<Texture>();
	UI_LB->Load("./Resources/UI/ui_2.png");
	resourceManager->AddTexture("UI_LB", UI_LB);
	std::shared_ptr<Texture> UI_RB = std::make_shared<Texture>();
	UI_RB->Load("./Resources/UI/ui_3.png");
	resourceManager->AddTexture("UI_RB", UI_RB);

	//シーン設定
	sceneManager->ChangeScene<GameScene>();

}

void Unique::OnFinalize() {
}
