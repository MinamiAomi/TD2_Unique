#include "Unique.h"

#include <memory>

#include "Scene/SceneManager.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Model.h"

#include "GameScene.h"

void Unique::OnInitialize() {
	SceneManager* sceneManager = SceneManager::GetInstance();
	//シーン設定
	sceneManager->ChangeScene<GameScene>();

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	resourceManager;
}

void Unique::OnFinalize() {
}
