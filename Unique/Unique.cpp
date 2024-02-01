#include "Unique.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Model.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Animation.h"

#include "GameScene.h"

void Unique::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();

	auto resourceManager = ResourceManager::GetInstance();
	resourceManager->AddModel("Cube", Model::Load("./Resources/cube/cube.obj"));
	resourceManager->AddModel("Gravity", Model::Load("./Resources/gravity/gravity.obj"));
	resourceManager->AddModel("Plane", Model::Load("./Resources/plane/plane.obj"));
	//天球
	resourceManager->AddModel("Skydome", Model::Load("./Resources/skydome/skydome.obj"));
	//武器モデル読み込み
	resourceManager->AddModel("Weapon", Model::Load("./Resources/weapon/weapon_3.obj"));
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
	//敵モデル読み込み
	resourceManager->AddModel("Enemy", Model::Load("./Resources/Enemy/Enemy.obj"));
	resourceManager->AddModel("Enemy_Barrier", Model::Load("./Resources/Enemy/EnemyBarrier.obj"));
	//UI読み込み
	std::shared_ptr<Texture> guradTex = std::make_shared<Texture>();
	guradTex->Load("./Resources/UI/Guard.png");
	resourceManager->AddTexture("guard_gauge", guradTex);

	std::shared_ptr<Texture> playerHpTex = std::make_shared<Texture>();
	playerHpTex->Load("./Resources/UI/HPbar_gage.png");
	resourceManager->AddTexture("player_hp", playerHpTex);

	std::shared_ptr<Texture> playerHpOverTex = std::make_shared<Texture>();
	playerHpOverTex->Load("./Resources/UI/HPbar_over.png");
	resourceManager->AddTexture("player_hp_over", playerHpOverTex);

	std::shared_ptr<Texture> playerHpUnderTex = std::make_shared<Texture>();
	playerHpUnderTex->Load("./Resources/UI/HPbar_under.png");
	resourceManager->AddTexture("player_hp_under", playerHpUnderTex);

	std::shared_ptr<Texture> enemyIndicatorTex = std::make_shared<Texture>();
	enemyIndicatorTex->Load("./Resources/UI/Enemy_indicator.png");
	resourceManager->AddTexture("indicator", enemyIndicatorTex);

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

	std::shared_ptr<Texture> UI_LS = std::make_shared<Texture>();
	UI_LS->Load("./Resources/UI/ui_ls.png");
	resourceManager->AddTexture("UI_LS", UI_LS);
	
	std::shared_ptr<Texture> UI_RS = std::make_shared<Texture>();
	UI_RS->Load("./Resources/UI/ui_rs.png");
	resourceManager->AddTexture("UI_RS", UI_RS);
	
	//エフェクト画像読み込み
	std::shared_ptr<Texture> hitEffect = std::make_shared<Texture>();
	hitEffect->Load("./Resources/Effect/hitEffect.png");
	resourceManager->AddTexture("hitEffect", hitEffect);

	HierarchicalAnimation::Load("./Resources/PlayerAnimation.gltf");

	//シーン設定
	sceneManager->ChangeScene<GameScene>();

}

void Unique::OnFinalize() {
}
