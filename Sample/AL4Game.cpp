#include "AL4Game.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/ToonModel.h"
#include "Graphics/Sprite.h"
#include "Graphics/Model.h"

#include "GameScene.h"

void AL4Game::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();
	//シーン設定
	sceneManager->ChangeScene<GameScene>();

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	
	auto model = Model::Load("Resources/Model/weapon.glb");

	std::shared_ptr<ToonModel> toonModel;
	std::shared_ptr<Texture> texture;

	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/sphere.obj"));
	resourceManager->AddToonModel("Sphere", toonModel);
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/box.obj"));
	resourceManager->AddToonModel("Box", toonModel);
	// 地面
	// プレイヤー
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/player.obj"));
	resourceManager->AddToonModel("Player", toonModel);
	// スカイドーム
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/skydome.obj"));
	resourceManager->AddToonModel("Skydome", toonModel);
	// 地面
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/ground.obj"));
	resourceManager->AddToonModel("Ground", toonModel);
	// ステージ床
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/floor.obj"));
	resourceManager->AddToonModel("Floor", toonModel);
	// 床
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/Goal.obj"));
	resourceManager->AddToonModel("Goal", toonModel);
	// 敵
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/mimic_body.obj"));
	resourceManager->AddToonModel("MimicBody", toonModel);
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/mimic_head.obj"));
	resourceManager->AddToonModel("MimicHead", toonModel);
	// 武器
	toonModel = std::make_shared<ToonModel>();
	toonModel->Create(ModelData::LoadObjFile("Resources/Model/weapon.obj"));
	resourceManager->AddToonModel("Weapon", toonModel);

	texture = std::make_shared<Texture>();
	texture->Load("Resources/lookOn.png");
	resourceManager->AddTexture("LookOn", texture);
}

void AL4Game::OnFinalize() {
}
