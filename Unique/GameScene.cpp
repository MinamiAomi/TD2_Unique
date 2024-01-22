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
    followCamera_->SetTarget(&player_->transform);
   /* enemy_->SetPlayer(player_.get());*/
    /*enemy_->SetBlockList(&blocks_);*/

}

void GameScene::Reset() {

    followCamera_->Initialize();
    player_->Initialize();
    /*enemy_->Initialize();*/
    stage_->Initialize();
    SmallEnemyManager::GetInstance()->Clear();
    enemies_.clear();

}

void GameScene::SetEnemy() {

    for (uint32_t i = 0; i < 10; i++) {

        std::shared_ptr<SmallEnemy> newEnemy = std::make_shared<SmallEnemy>();
        newEnemy->Initialize({ randomNumberGenerator.NextFloatRange(-40.0f,40.0f),
        0.0f, randomNumberGenerator.NextFloatRange(-40.0f, 40.0f), });
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

#endif // _DEBUG


    enemies_.remove_if([](auto& enemy) {

        if (enemy->GetIsDead()) {
            SmallEnemyManager::GetInstance()->DeleteEnemy(enemy.get());
            return true;
        }

        return false;

       });

    GlobalVariables::GetInstance()->Update();

    Input* input = Input::GetInstance();

    if (input->IsKeyTrigger(DIK_R)) {
        Reset();
    }

    //敵召喚
    if (input->IsKeyTrigger(DIK_E)) {
        SetEnemy();
    }

    for (auto& enemy : enemies_) {
        enemy->Update();
    }

    player_->Update();
   /* enemy_->Update();*/
    stage_->Update();

    CollisionManager::GetInstance()->CheckCollision();

    followCamera_->Update();

    sunLight_->DrawImGui("SunLight");

}

void GameScene::OnFinalize() {
}
