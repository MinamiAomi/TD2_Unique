#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Graphics/ResourceManager.h"
#include "Collision/CollisionManager.h"
#include "Game/enemy/EnemyCoreManager.h"
#include "GlobalVariables.h"
#include "Game/enemy/SmallEnemyManager.h"
#include "Math/Random.h"
#include "Graphics/ImGuiManager.h"

static Random::RandomNumberGenerator randomNumberGenerator;

void GameScene::OnInitialize() {

#ifdef _DEBUG

    editor_ = MapEditor::GetInstance();
    editor_->Initialize();

#endif // _DEBUG

    GlobalVariables::GetInstance()->LoadFiles();

    followCamera_ = std::make_shared<FollowCamera>();
    player_ = std::make_shared<Player>();
    stage_ = std::make_shared<Stage>();
    reticleTex_ = ResourceManager::GetInstance()->FindTexture("reticle");
    reticle_ = std::make_unique<Sprite>();
    /*enemy_ = std::make_shared<Enemy>();*/

    EnemyCoreManager::GetInstance()->Clear();

    followCamera_->Initialize();
    player_->Initialize();
    stage_->Initialize();
   /* enemy_->Initialize();*/

    RenderManager::GetInstance()->SetCamera(followCamera_->GetCamera());
    sunLight_ = std::make_shared<DirectionalLight>();
    RenderManager::GetInstance()->SetSunLight(sunLight_);

    //セット
    player_->SetCamera(followCamera_);
    followCamera_->SetTarget(player_->playerTransforms_[Player::kHip].get());
  

}

void GameScene::Reset() {

    followCamera_->Initialize();
    player_->Initialize();
    /*enemy_->Initialize();*/
    stage_->Initialize();

}

void GameScene::SetEnemy(uint32_t num) {

    for (uint32_t i = 0; i < num; i++) {

        std::shared_ptr<SmallEnemy> newEnemy = std::make_shared<BarrierEnemy>();
        newEnemy->Initialize({ randomNumberGenerator.NextFloatRange(-200.0f,200.0f),
        10.0f, randomNumberGenerator.NextFloatRange(-200.0f, 200.0f), });
        newEnemy->SetPlayer(player_.get());
        SmallEnemyManager::GetInstance()->AddEnemy(newEnemy);
        enemies_.push_back(newEnemy);

    }

}

void GameScene::OnUpdate() {

#ifdef _DEBUG

    //操作説明表記
    ImGui::Begin("Command");
    ImGui::Text("Player");
    ImGui::Text("R Button : Collect");
    ImGui::Text("R Trigger : Shot");
    ImGui::Text("L Button : Dash");
    ImGui::Text("A Button : Attack");

    ImGui::End();

    editor_->Edit();

#endif // _DEBUG

    GlobalVariables::GetInstance()->Update();

    /*Input* input = Input::GetInstance();*/

    player_->Update();
   /* enemy_->Update();*/
    stage_->Update();

    CollisionManager::GetInstance()->CheckCollision();

    followCamera_->Update();

    sunLight_->DrawImGui("SunLight");

}

void GameScene::OnFinalize() {
}
