#include "GameScene.h"

#include "Collision/CollisionManager.h"
#include "GlobalVariables.h"

void GameScene::OnInitialize() {
    GlobalVariables::GetInstance()->LoadFiles();
    // 生成
    skydome_ = std::make_shared<Skydome>();
    player_ = std::make_shared<Player>();
    followCamera_ = std::make_shared<FollowCamera>();
    //ground_ = std::make_shared<Ground>();
    stage_ = std::make_shared<Stage>();
    lookOn_ = std::make_shared<LookOn>();
    
    // 初期化
    skydome_->Initialize();
    stage_->Initialize();
    player_->Initialize();
    followCamera_->Initialize();
    lookOn_->Initialize();

    // セット
    player_->SetCamera(followCamera_);
    followCamera_->SetTarget(&player_->transform);
    followCamera_->SetLookOn(lookOn_);
}

void GameScene::OnUpdate() {
    GlobalVariables::GetInstance()->Update();

    if (player_->RequestRestart()) {
        player_->Restart();
        stage_->Restart();
        followCamera_->Restart();
        lookOn_->Restart();
    }

    // 更新
    stage_->Update();
    player_->Update();
    skydome_->Update();

    // 当たり判定を取る
    CollisionManager::GetInstance()->CheckCollision();

    lookOn_->Update(stage_->GetEnemies(), *followCamera_->GetCamera());
    followCamera_->Update();
}

void GameScene::OnFinalize() {
   // CollisionManager::GetInstance()->ClearCollider();
}
