#include "GameScene.h"

#include "Input/Input.h"
#include "Graphics/RenderManager.h"
#include "Graphics/ResourceManager.h"
#include "Collision/CollisionManager.h"
#include "Game/enemy/EnemyCoreManager.h"
#include "GlobalVariables.h"
#include "Game/enemy/SmallEnemyManager.h"
#include "Graphics/ImGuiManager.h"
#include "Externals/nlohmann/json.hpp"

void GameScene::OnInitialize() {

#ifdef _DEBUG

#endif // _DEBUG

    GlobalVariables::GetInstance()->LoadFiles();

    followCamera_ = std::make_shared<FollowCamera>();
    player_ = std::make_shared<Player>();
    stage_ = std::make_shared<Stage>();
    reticleTex_ = ResourceManager::GetInstance()->FindTexture("reticle");
    reticle_ = std::make_unique<Sprite>();
    /*enemy_ = std::make_shared<Enemy>();*/
    editor_ = MapEditor::GetInstance();
    editor_->Initialize();

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
    SmallEnemyManager::GetInstance()->Clear();
    enemies_.clear();
    waveNumber_ = 0;

}

void GameScene::SetEnemy(const std::string& tag, const Vector3& position) {

    if (tag == "Normal") {

        std::shared_ptr<SmallEnemy> newEnemy = std::make_shared<SmallEnemy>();
        newEnemy->Initialize(position);
        newEnemy->SetPlayer(player_.get());
        SmallEnemyManager::GetInstance()->AddEnemy(newEnemy);
        enemies_.push_back(newEnemy);

    }
    else if (tag == "Barrier") {

        std::shared_ptr<SmallEnemy> newEnemy = std::make_shared<BarrierEnemy>();
        newEnemy->Initialize(position);
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


    enemies_.remove_if([](auto& enemy) {

        if (enemy->GetIsDead()) {
            SmallEnemyManager::GetInstance()->DeleteEnemy(enemy.get());
            return true;
        }

        return false;

        });

    //空になったらウェーブ進行、次のデータに沿って敵を配置
    if (enemies_.empty()) {

        //最大ウェーブ数までロード
        if (waveNumber_ < kMaxWave_) {
            waveNumber_++;
            LoadEnemyPopData(waveNumber_);
        }

    }


    GlobalVariables::GetInstance()->Update();

    Input* input = Input::GetInstance();

    if (input->IsKeyTrigger(DIK_R) || player_->GetIsDead()) {
        Reset();
    }

    if (input->IsKeyTrigger(DIK_C)) {

        //コライダーを非アクティブ(ctrl + C)
        if (input->IsKeyPressed(DIK_LCONTROL)) {
            for (auto& enemy : enemies_) {
                enemy->GetCollider()->SetIsActive(false);
            }
        }
        //コライダーをアクティブ(shift + C)
        else if (input->IsKeyPressed(DIK_LSHIFT)) {
            for (auto& enemy : enemies_) {
                enemy->GetCollider()->SetIsActive(true);
            }
        }

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


void GameScene::LoadEnemyPopData(uint32_t waveNumber) {

    std::string filename = "wave";

    filename += std::to_string(waveNumber);

    //読み込むJSONファイルのフルパスを合成する
    std::string filePath = "./resources/EnemySpawn/" + filename + ".json";
    //読み込み用ファイルストリーム
    std::ifstream ifs;
    //ファイルを読み込み用に開く
    ifs.open(filePath);

    //ファイルオープン失敗したら表示
    if (ifs.fail()) {
        MessageBox(nullptr, L"スポーンデータが存在しません。", L"Map Editor - Load", 0);
        return;
    }

    nlohmann::json root;

    //json文字列からjsonのデータ構造に展開
    ifs >> root;
    //ファイルを閉じる
    ifs.close();
    //グループを検索
    nlohmann::json::iterator itGroup = root.find(filename);
    //未登録チェック
    if (itGroup == root.end()) {
        MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
        return;
    }

    //保険
    assert(itGroup != root.end());

    //各アイテムについて
    for (nlohmann::json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {

        //アイテム名を取得
        const std::string& itemName = itItem.key();

        //グループを検索
        nlohmann::json::iterator itObject = itGroup->find(itemName);

        //未登録チェック
        if (itObject == itGroup->end()) {
            MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
            return;
        }

        //保険
        assert(itObject != itGroup->end());

        //アイテム名がオブジェクトデータだった場合、登録
        if (itemName == "objectData") {

            //各オブジェクトについて
            for (nlohmann::json::iterator itObjectData = itObject->begin(); itObjectData != itObject->end(); ++itObjectData) {

                //アイテム名を取得
                const std::string& objectName = itObjectData.key();

                //グループを検索
                nlohmann::json::iterator itData = itObject->find(objectName);

                //未登録チェック
                if (itData == itObject->end()) {
                    MessageBox(nullptr, L"ファイルの構造が正しくありません。", L"Map Editor - Load", 0);
                    return;
                }

                //保険
                assert(itData != itObject->end());

                std::shared_ptr<MapObject> mapObject = std::make_shared<MapObject>();

                mapObject->isSelect = true;
                mapObject->model = std::make_shared<ModelInstance>();
                mapObject->model->SetModel(ResourceManager::GetInstance()->FindModel("Cube"));
                mapObject->objName = objectName;
                mapObject->transform = std::make_shared<Transform>();
                mapObject->transform->rotate = Quaternion::identity;
                mapObject->transform->scale = Vector3::one * 2.0f;

                uint32_t roopCount = 0;

                for (nlohmann::json::iterator itItemObject = itData->begin(); itItemObject != itData->end(); ++itItemObject) {

                    //アイテム名を取得
                    const std::string& itemNameObject = itItemObject.key();

                    //要素数3の配列であれば
                    if (itItemObject->is_array() && itItemObject->size() == 3) {

                        //名前がpositionだった場合、positionを登録
                        if (itemNameObject == "position") {
                            //float型のjson配列登録
                            mapObject->transform->translate = { itItemObject->at(0), itItemObject->at(1), itItemObject->at(2) };
                        }


                    }
                    //Vector3以外の場合
                    else {

                        //タグを登録
                        if (itemNameObject == "tag") {
                            mapObject->tag = itItemObject.value();
                        }
                        else if (itemNameObject == "tagNumber") {
                            mapObject->tagNumber = itItemObject->get<int32_t>();
                        }


                    }

                    roopCount++;

                }

                SetEnemy(mapObject->tag, mapObject->transform->translate);

            }

        }
    }

}


