#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Graphics/ResourceManager.h"
#include "Collision/CollisionManager.h"

void GameScene::OnInitialize() {

    followCamera_ = std::make_shared<FollowCamera>();
    player_ = std::make_shared<Player>();
    stage_ = std::make_shared<Stage>();
    reticleTex_ = ResourceManager::GetInstance()->FindTexture("reticle");
    reticle_ = std::make_unique<Sprite>();
    enemy_ = std::make_shared<Enemy>();

    followCamera_->Initialize();
    player_->Initialize();
    stage_->Initialize();
    enemy_->Initialize();


    RenderManager::GetInstance()->SetCamera(followCamera_->GetCamera());
    sunLight_ = std::make_shared<DirectionalLight>();
    RenderManager::GetInstance()->SetSunLight(sunLight_);

    //セット
    player_->SetCamera(followCamera_);
    followCamera_->SetTarget(&player_->transform);
    enemy_->SetPlayer(player_.get());
    enemy_->SetBlockList(&blocks_);

}

void GameScene::Reset() {

    player_->Initialize();
    enemy_->Initialize();
    stage_->Initialize();

}

void GameScene::OnUpdate() {

    Input* input = Input::GetInstance();

    if (input->IsKeyTrigger(DIK_R)) {
        Reset();
    }

    player_->Update();
    enemy_->Update();
    stage_->Update();

    CollisionManager::GetInstance()->CheckCollision();

    followCamera_->Update();

    sunLight_->DrawImGui("SunLight");

}

void GameScene::OnFinalize() {
}
