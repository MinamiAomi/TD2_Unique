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
#include "Game/enemy/BarrierBulletManager.h"
#include "Game/enemy/BulletManager.h"

void GameScene::OnInitialize() {

#ifdef _DEBUG

#endif // _DEBUG

    GlobalVariables::GetInstance()->LoadFiles();
    audio_ = Audio::GetInstance();
    hitStopManager_ = HitStopManager::GetInstance();

    followCamera_ = std::make_shared<FollowCamera>();
    player_ = std::make_shared<Player>();
    stage_ = std::make_shared<Stage>();
    editor_ = MapEditor::GetInstance();
    editor_->Initialize();
    blackSprite_ = std::make_unique<Sprite>();
    blackSprite_->SetScale({ 1280.0f,720.0f });
    blackSprite_->SetPosition({ 640.0f,360.0f });
    blackSprite_->SetColor({ 0.0f,0.0f,0.0f,0.1f });
    blackSprite_->SetDrawOrder(99);

    whiteSprite_ = std::make_unique<Sprite>();
    whiteSprite_->SetScale({ 1280.0f,720.0f });
    whiteSprite_->SetPosition({ 640.0f,360.0f });
    whiteSprite_->SetColor({ 0.0f,0.0f,0.0f,0.1f });
    whiteSprite_->SetDrawOrder(100);

    titleTex_ = ResourceManager::GetInstance()->FindTexture("title");
    titleSprite_ = std::make_unique<Sprite>();
    titleSprite_->SetTexture(titleTex_);
    titleSprite_->SetTexcoordRect({ 0.0f,0.0f, }, { 3072.0f,512.0f });
    titleSprite_->SetPosition({ 640.0f,600.0f });
    
    titleMove_.titleScale_ = { 768.0f,128.0f };
    titleSprite_->SetScale(titleMove_.titleScale_);
    titleMove_.titleAlpha_ = 1.0f;
    titleSprite_->SetColor({ 1.0f,1.0f,1.0f,titleMove_.titleAlpha_ });

    startTex_ = ResourceManager::GetInstance()->FindTexture("start");
    startSprite_ = std::make_unique<Sprite>();
    startSprite_->SetTexture(startTex_);
    startSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 700.0f, 200.0f });
    startSprite_->SetPosition({ 640.0f,100.0f });
    startSprite_->SetScale({ 280.0f,80.0f });

    toTitleTex_ = ResourceManager::GetInstance()->FindTexture("ToTitle");
    toTitleSprite_ = std::make_unique<Sprite>();
    toTitleSprite_->SetTexture(toTitleTex_);
    toTitleSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 800.0f, 200.0f });
    toTitleSprite_->SetPosition({ 640.0f,100.0f });
    toTitleSprite_->SetScale({ 320.0f,80.0f });
    toTitleSprite_->SetIsActive(false);

    gameOverTex_ = ResourceManager::GetInstance()->FindTexture("GameOver");
    gameOverSprite_ = std::make_unique<Sprite>();
    gameOverSprite_->SetTexture(gameOverTex_);
    gameOverSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 1208.0f, 581.0f });
    gameOverSprite_->SetPosition({ 640.0f,600.0f });
    gameOverSprite_->SetScale({ 604.0f,290.5f });
    gameOverSprite_->SetIsActive(false);

    clearTex_ = ResourceManager::GetInstance()->FindTexture("GameClear");
    clearSprite_ = std::make_unique<Sprite>();
    clearSprite_->SetTexture(clearTex_);
    clearSprite_->SetTexcoordRect({ 0.0f,0.0f }, { 1208.0f, 581.0f });
    clearSprite_->SetPosition({ 640.0f,600.0f });
    clearSprite_->SetScale({ 604.0f,290.5f });
    clearSprite_->SetIsActive(false);

    EnemyCoreManager::GetInstance()->Clear();

    followCamera_->Initialize();
    player_->Initialize();
    player_->SetIsStart(false);
    player_->Update();
    stage_->Initialize();

    RenderManager::GetInstance()->SetCamera(followCamera_->GetCamera());
    sunLight_ = std::make_shared<DirectionalLight>();
    sunLight_->direction = Vector3(0.01f, -1.0f, 0.01f).Normalized();
    RenderManager::GetInstance()->SetSunLight(sunLight_);

    //セット
    player_->SetCamera(followCamera_);
    followCamera_->SetTarget(&player_->transform);
    followCamera_->Update();

    editorCamera_ = std::make_shared<Camera>();
    editorCameraTransform_ = std::make_shared<Transform>();
    editorCameraTransform_->translate = { 0.0f,150.0f,0.0f };
    editorCameraTransform_->rotate = Quaternion::MakeFromAngleAxis(1.57f, Vector3{ 1.0f,0.0f,0.0f }.Normalized());
    editorCameraTransform_->UpdateMatrix();
    editorCamera_->SetPosition(editorCameraTransform_->translate);
    editorCamera_->SetRotate(editorCameraTransform_->rotate);

    enemyBGM_ = audio_->SoundLoadWave("./Resources/sound/zakoBGM.wav");
    bossBGM_ = audio_->SoundLoadWave("./Resources/sound/bossBGM.wav");
    titleBGM_ = audio_->SoundLoadWave("./Resources/sound/titleBGM.wav");
    selectSE_ = audio_->SoundLoadWave("./Resources/sound/startselect.wav");

#ifdef _DEBUG

    /*RenderManager::GetInstance()->SetCamera(editorCamera_);*/

#endif // _DEBUG


}

void GameScene::FadeInOut() {

    if (fadeIn_) {
        fadeAlpha_ += 0.025f;
    }
    else if (fadeOut_) {
        fadeAlpha_ -= 0.025f;
    }

    if (fadeAlpha_ >= 1.0f && fadeIn_) {

        if (nextScene_ == kTitle) {
            ResetTitle();
        }
        else if (nextScene_ == kInGame) {
            ResetInGame();
        }

        fadeIn_ = false;
        fadeOut_ = true;

    }
    else if (fadeOut_ && fadeAlpha_ <= 0.0f) {

        fadeOut_ = false;
        isFade_ = false;

    }

    blackSprite_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });

}

void GameScene::ResetTitle() {

    audio_->SoundPlayLoopEnd(bossBGMHandle_);
    audio_->SoundPlayLoopEnd(enemyBGMHandle_);

    titleSprite_->SetScale(titleMove_.titleScale_);
    titleSprite_->SetColor({ 1.0f,1.0f,1.0f,titleMove_.titleAlpha_ });

    isBossBattle_ = false;
    player_->Initialize();
    player_->SetIsStart(false);
    player_->Update();
    followCamera_->Initialize();
    followCamera_->SetTarget(&player_->transform);
    followCamera_->Update();
    EnemyCoreManager::GetInstance()->Clear();
    BarrierBulletManager::GetInstance()->Clear();
    BulletManager::GetInstance()->Clear();
    enemy_.reset();
    SmallEnemyManager::GetInstance()->Clear();
    enemies_.clear();
    waveNumber_ = 1;
    titleMove_.sceneChangeTimer_ = titleMove_.maxChangeTime_;
    titleMove_.isSceneChange_ = false;
    isTitle_ = true;

}

void GameScene::ResetInGame() {

    audio_->SoundPlayLoopEnd(bossBGMHandle_);
    audio_->SoundPlayLoopEnd(enemyBGMHandle_);

    isBossBattle_ = false;
    player_->Initialize();
    player_->SetIsStart(true);
    player_->Update();
    followCamera_->Initialize();
    followCamera_->Update();
    EnemyCoreManager::GetInstance()->Clear();
    BarrierBulletManager::GetInstance()->Clear();
    BulletManager::GetInstance()->Clear();
    enemy_.reset();
    /*stage_->Initialize();*/
    SmallEnemyManager::GetInstance()->Clear();
    enemies_.clear();
    waveNumber_ = 1;
    isTitle_ = false;
    enemyBGMHandle_ = audio_->SoundPlayLoopStart(enemyBGM_);
    audio_->SetValume(enemyBGMHandle_, 0.5f);
}

void GameScene::SetEnemy(const std::string& tag, const Vector3& position) {

    if (tag == "Normal") {

        std::shared_ptr<SmallEnemy> newEnemy = std::make_shared<SmallEnemy>();
        newEnemy->Initialize(position, SmallEnemy::kHoming);
        newEnemy->SetPlayer(player_.get());
        SmallEnemyManager::GetInstance()->AddEnemy(newEnemy);
        enemies_.push_back(newEnemy);

    }
    else if (tag == "Barrier") {

        std::shared_ptr<SmallEnemy> newEnemy = std::make_shared<BarrierEnemy>();
        newEnemy->Initialize(position, SmallEnemy::kHoming);
        newEnemy->SetPlayer(player_.get());
        SmallEnemyManager::GetInstance()->AddEnemy(newEnemy);
        enemies_.push_back(newEnemy);

    }

}

void GameScene::BossSpawn() {

    enemy_ = std::make_unique<Enemy>();
    enemy_->Initialize();
    enemy_->SetPlayer(player_.get());

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
        ImGui::Text("shift + 1 or 2 : Change Camera");
        ImGui::EndMenu();
    }

    ImGui::Spacing();
    ImGui::EndMenuBar();
    ImGui::End();

    
#endif // _DEBUG


}

void GameScene::OnUpdate() {

    Input* input = Input::GetInstance();

    auto& xinputState = input->GetXInputState();

    auto& preXInputState = input->GetPreXInputState();

#ifdef _DEBUG

    Manual();

    //editor_->Edit();

    ////カメラの移動、切り替え処理
    //EditorCameraMove();

    if (input->IsKeyTrigger(DIK_R)) {
        ResetInGame();
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

#endif // _DEBUG

    if (++frameTimer_ >= 120) {
        frameTimer_ = 0;
    }

    //ヒットストップしていない時に更新
    if (isFade_) {

        FadeInOut();

    }
    else if (isTitle_) {

        if (!titleMove_.isSceneChange_ && !audio_->IsValidPlayHandle(titleBGMHandle_)) {
            titleBGMHandle_ = audio_->SoundPlayLoopStart(titleBGM_);
            audio_->SetValume(titleBGMHandle_, 0.5f);
        }

        if (!titleMove_.isSceneChange_ && 
            (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
            !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
            audio_->SoundPlayLoopEnd(titleBGMHandle_);
            titleBGMHandle_ = UNUSED_PLAY_HANDLE;
            audio_->SoundPlayWave(selectSE_);
            titleMove_.sceneChangeTimer_ = titleMove_.maxChangeTime_;
            titleMove_.isSceneChange_ = true;
        }

        if (titleMove_.isSceneChange_) {
            titleMove_.sceneChangeTimer_--;

            if (titleMove_.sceneChangeTimer_ >= 110) {
                Vector2 lerp = Vector2::Slerp(1.0f - float((titleMove_.sceneChangeTimer_ - 110) / 10.0f),
                    titleMove_.titleScale_, titleMove_.titleScaleAfter_);

                titleSprite_->SetScale(lerp);
            }
            
        }

        if (titleMove_.isSceneChange_ && titleMove_.sceneChangeTimer_ <= 0) {
            titleSprite_->SetScale({ 0.0f,0.0f });
            ResetInGame();
            isTitle_ = false;
            titleMove_.isSceneChange_ = false;
        }

        player_->Update();

        stage_->Update();

    }
    else {

        if (hitStopManager_->GetCount() <= 0) {

            enemies_.remove_if([](auto& enemy) {

                if (enemy->GetIsDead()) {
                    SmallEnemyManager::GetInstance()->DeleteEnemy(enemy.get());
                    return true;
                }

                return false;

                });

            //空になったらウェーブ進行、次のデータに沿って敵を配置
            if (CheckEnemiesDead() && !isBossBattle_) {

                //最大ウェーブ数までロード
                if (waveNumber_ <= kMaxWave_) {
                    LoadEnemyPopData(waveNumber_);
                    waveNumber_++;
                }
                else {
                    audio_->StopSound(enemyBGMHandle_);
                    bossBGMHandle_ = audio_->SoundPlayLoopStart(bossBGM_);
                    audio_->SetValume(bossBGMHandle_, 0.5f);
                    BossSpawn();
                    isBossBattle_ = true;
                }

            }


            GlobalVariables::GetInstance()->Update();

            //プレイヤーが死んだ場合
            if (player_->GetIsDead()) {

                player_->SetIsStart(false);

                fadeAlpha_ = 0.3f;
                blackSprite_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });

                if (!isFade_ && (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
                    !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
                    nextScene_ = kTitle;
                    audio_->SoundPlayWave(selectSE_);
                    fadeIn_ = true;
                    isFade_ = true;
                }

            }
            //敵を倒した場合
            else if (enemy_ && enemy_->GetIsDead()) {

                whiteSprite_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });
                player_->SetIsStart(false);

                if (!isFade_ && (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) &&
                    !(preXInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
                    nextScene_ = kTitle;
                    audio_->SoundPlayWave(selectSE_);
                    fadeIn_ = true;
                    isFade_ = true;
                }

            }

            player_->Update();

            if (!player_->GetIsDead()) {

                followCamera_->Update();

                for (auto& enemy : enemies_) {
                    enemy->Update();
                }

                if (enemy_) {
                    enemy_->Update();
                }

            }

            stage_->Update();

            CollisionManager::GetInstance()->CheckCollision();

            

        }
        else {

            hitStopManager_->SubHitStopFrame();

        }

    }

    if (titleMove_.isSceneChange_ && titleMove_.sceneChangeTimer_ > 80) {

        if (frameTimer_ % 2 == 0) {
            startSprite_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
        }
        else {
            startSprite_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
        }

    }
    else if (titleMove_.isSceneChange_) {
        startSprite_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    }
    else if (frameTimer_ % 120 < 30) {
        float alpha = (30.0f - float(frameTimer_ % 120)) / 30.0f;
        startSprite_->SetColor({ 1.0f,1.0f,1.0f, alpha });
        toTitleSprite_->SetColor({ 1.0f,1.0f,1.0f, alpha });
    }
    else if (frameTimer_ % 120 < 60) {
        float alpha = (float(frameTimer_ % 120) - 30.0f) / 30.0f;
        startSprite_->SetColor({ 1.0f,1.0f,1.0f, alpha });
        toTitleSprite_->SetColor({ 1.0f,1.0f,1.0f, alpha });
    }

    if (isTitle_) {
        titleSprite_->SetIsActive(true);
        startSprite_->SetIsActive(true);
        toTitleSprite_->SetIsActive(false);
    }
    else if (player_->GetIsDead() || (enemy_ && enemy_->GetIsDead())) {
        titleSprite_->SetIsActive(false);
        startSprite_->SetIsActive(false);
        toTitleSprite_->SetIsActive(true);
    }
    else {
        titleSprite_->SetIsActive(false);
        startSprite_->SetIsActive(false);
        toTitleSprite_->SetIsActive(false);
    }

    if ((enemy_ && enemy_->GetIsDead()) && !player_->GetIsDead()) {
        whiteSprite_->SetIsActive(true);
        clearSprite_->SetIsActive(true);
    }
    else {
        whiteSprite_->SetIsActive(false);
        clearSprite_->SetIsActive(false);
    }

    if (player_->GetIsDead()) {
        gameOverSprite_->SetIsActive(true);
    }
    else {
        gameOverSprite_->SetIsActive(false);
    }
    if ((isFade_ || player_->GetIsDead())) {
        blackSprite_->SetIsActive(true);
    }
    else {
        blackSprite_->SetIsActive(false);
    }

}

void GameScene::OnFinalize() {
}

void GameScene::EditorCameraMove() {

    Input* input = Input::GetInstance();

    //エディター用のカメラに設定
    if (input->IsKeyTrigger(DIK_1) && (input->IsKeyPressed(DIK_LSHIFT) || input->IsKeyPressed(DIK_RSHIFT))) {
        RenderManager::GetInstance()->SetCamera(editorCamera_);
    }
    //プレイヤー目線のカメラに設定
    else if (input->IsKeyTrigger(DIK_2) && (input->IsKeyPressed(DIK_LSHIFT) || input->IsKeyPressed(DIK_RSHIFT))) {
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

bool GameScene::CheckEnemiesDead() {

    for (auto& enemy : enemies_) {

        //重力に影響されていなかったら生きている判定
        if (!enemy->GetIsAffectedGravity()) {
            return false;
        }

    }

    return true;

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


