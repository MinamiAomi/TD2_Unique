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
  
    editorCamera_ = std::make_shared<Camera>();
    editorCameraTransform_ = std::make_shared<Transform>();
    editorCameraTransform_->translate = { 0.0f,150.0f,0.0f };
    editorCameraTransform_->rotate = Quaternion::MakeFromAngleAxis(1.57f, Vector3{ 1.0f,0.0f,0.0f }.Normalized());
    editorCameraTransform_->UpdateMatrix();
    editorCamera_->SetPosition(editorCameraTransform_->translate);
    editorCamera_->SetRotate(editorCameraTransform_->rotate);

#ifdef _DEBUG

    RenderManager::GetInstance()->SetCamera(editorCamera_);

#endif // _DEBUG


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

void GameScene::Manual() {

#ifdef _DEBUG

    //操作説明表記
    if (!ImGui::Begin("Commands", nullptr, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    if (!ImGui::BeginMenuBar()) {
        return;
    }

    //プレイヤー
    if (ImGui::BeginMenu("Player")) {

        ImGui::Text("R Button : Collect");
        ImGui::Text("R Trigger : Shot");
        ImGui::Text("L Button : Dash");
        ImGui::Text("A Button : Attack");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Camera")) {
        ImGui::Text("A or D : Move X");
        ImGui::Text("W or S : Move Z");
        ImGui::Text("Q or E : Move Y");
        ImGui::Text("1 or 2 : Change Camera");
        ImGui::EndMenu();
    }

    ImGui::Spacing();
    ImGui::EndMenuBar();
    ImGui::End();

#endif // _DEBUG


}

void GameScene::OnUpdate() {

#ifdef _DEBUG

    Manual();

    editor_->Edit();

    //カメラの移動、切り替え処理
    EditorCameraMove();

#endif // _DEBUG

    GlobalVariables::GetInstance()->Update();

    player_->Update();
   /* enemy_->Update();*/
    stage_->Update();

    CollisionManager::GetInstance()->CheckCollision();

    followCamera_->Update();

    sunLight_->DrawImGui("SunLight");

}

void GameScene::OnFinalize() {
}

void GameScene::EditorCameraMove() {

    Input* input = Input::GetInstance();

    //エディター用のカメラに設定
    if (input->IsKeyTrigger(DIK_1)) {
        RenderManager::GetInstance()->SetCamera(editorCamera_);
    }
    //プレイヤー目線のカメラに設定
    else if (input->IsKeyTrigger(DIK_2)) {
        RenderManager::GetInstance()->SetCamera(followCamera_->GetCamera());
    }

    //移動
    if (input->IsKeyPressed(DIK_W) || input->IsKeyPressed(DIK_UP)) {
        editorCameraTransform_->translate.z += 1.0f;
    }

    if (input->IsKeyPressed(DIK_S) || input->IsKeyPressed(DIK_DOWN)) {
        editorCameraTransform_->translate.z -= 1.0f;
    }

    if (input->IsKeyPressed(DIK_A) || input->IsKeyPressed(DIK_LEFT)) {
        editorCameraTransform_->translate.x -= 1.0f;
    }

    if (input->IsKeyPressed(DIK_D) || input->IsKeyPressed(DIK_RIGHT)) {
        editorCameraTransform_->translate.x += 1.0f;
    }

    if (input->IsKeyPressed(DIK_Q) && editorCameraTransform_->translate.y < upperLimit_) {
        editorCameraTransform_->translate.y += 1.0f;
    }

    if (input->IsKeyPressed(DIK_E) && editorCameraTransform_->translate.y > lowerLimit_) {
        editorCameraTransform_->translate.y -= 1.0f;
    }

    editorCameraTransform_->UpdateMatrix();
    editorCamera_->SetPosition(editorCameraTransform_->translate);
    editorCamera_->SetRotate(editorCameraTransform_->rotate);
    editorCamera_->UpdateMatrices();

}
