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
	resourceManager->AddModel("Sphere", Model::Load("./Resources/gravity/gravity.obj"));
	//武器モデル読み込み
	resourceManager->AddModel("Weapon", Model::Load("./Resources/weapon/weapon_2.obj"));
	resourceManager->AddModel("Weapon_Head", Model::Load("./Resources/weapon/weapon_head_2.obj"));
	//プレイヤーモデル読み込み
	resourceManager->AddModel("Body", Model::Load("./Resources/PlayerModel_2/Body.obj"));
	resourceManager->AddModel("Head", Model::Load("./Resources/PlayerModel_2/Head.obj"));
	resourceManager->AddModel("Hip", Model::Load("./Resources/PlayerModel_2/Hip.obj"));
	resourceManager->AddModel("LeftLowerArm", Model::Load("./Resources/PlayerModel_2/LeftLowerArm.obj"));
	resourceManager->AddModel("LeftLowerLeg", Model::Load("./Resources/PlayerModel_2/LeftLowerLeg.obj"));
	resourceManager->AddModel("LeftShoulder", Model::Load("./Resources/PlayerModel_2/LeftShoulder.obj"));
	resourceManager->AddModel("LeftUpperArm", Model::Load("./Resources/PlayerModel_2/LeftUpperArm.obj"));
	resourceManager->AddModel("LeftUpperLeg", Model::Load("./Resources/PlayerModel_2/LeftUpperLeg.obj"));
	resourceManager->AddModel("RightLowerArm", Model::Load("./Resources/PlayerModel_2/RightLowerArm.obj"));
	resourceManager->AddModel("RightLowerLeg", Model::Load("./Resources/PlayerModel_2/RightLowerLeg.obj"));
	resourceManager->AddModel("RightShoulder", Model::Load("./Resources/PlayerModel_2/RightShoulder.obj"));
	resourceManager->AddModel("RightUpperArm", Model::Load("./Resources/PlayerModel_2/RightUpperArm.obj"));
	resourceManager->AddModel("RightUpperLeg", Model::Load("./Resources/PlayerModel_2/RightUpperLeg.obj"));
	//UI読み込み
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
	std::shared_ptr<Texture> UI_RT = std::make_shared<Texture>();
	UI_RT->Load("./Resources/UI/ui_rt.png");
	resourceManager->AddTexture("UI_RT", UI_RT);

	//シーン設定
	sceneManager->ChangeScene<GameScene>();

}

void Unique::OnFinalize() {
}
