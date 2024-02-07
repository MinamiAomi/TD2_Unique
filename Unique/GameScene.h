#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Math/Camera.h"
#include "Math/Transform.h"
#include "Math/Random.h"
#include "Graphics/Model.h"
#include "Graphics/LightManager.h"
#include "Game/player/Player.h"
#include "Game/stage/Stage.h"
#include "Game/enemy/Enemy.h"
#include "Game/block/Block.h"
#include "Game/followCamera/FollowCamera.h"
#include "Game/enemy/SmallEnemy.h"
#include "App/MapEditor.h"
#include "Game/HitStop/HitStopManager.h"

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:

    void ResetTitle();

    void ResetInGame();

    void Manual();

    void SetEnemy(const std::string& tag, const Vector3& position);

    void LoadEnemyPopData(uint32_t waveNumber);

    void EditorCameraMove();

    void FadeInOut();

    void BossSpawn();

    bool CheckEnemiesDead();

private:

    Audio* audio_ = nullptr;

    enum NextScene {
        kTitle,
        kInGame,
    };

    MapEditor* editor_ = nullptr;
    HitStopManager* hitStopManager_ = nullptr;

    struct Spawn {
        //自動スポーンに利用する変数
        int32_t spawnTimer = 0;
        //スポーンまでの時間
        int32_t spawnFrame = 120;
        //スポーン数
        uint32_t spawnCount = 5;
    };

    Vector3 euler_;
    std::shared_ptr<DirectionalLight> sunLight_;

    std::shared_ptr<FollowCamera> followCamera_;

    std::shared_ptr<Player> player_;

    std::shared_ptr<Camera> editorCamera_;

    std::shared_ptr<Transform> editorCameraTransform_;

    //エディターカメラのY軸上制限
    float upperLimit_ = 300.0f;
    //エディターカメラのY軸下制限
    float lowerLimit_ = 15.0f;

    std::shared_ptr<Stage> stage_;

    std::list<std::shared_ptr<SmallEnemy>> enemies_;

    Spawn spawn_;

    std::unique_ptr<Enemy> enemy_;

    Vector3 reticlePos_{};

    uint32_t waveNumber_ = 1;

    uint32_t kMaxWave_ = 4;

    NextScene nextScene_ = kInGame;

    bool isTitle_ = true;

    bool isBossBattle_ = false;

    bool isFade_ = false;

    bool fadeIn_ = false;
    bool fadeOut_ = false;

    std::unique_ptr<Sprite> blackSprite_;
    std::unique_ptr<Sprite> whiteSprite_;
    std::shared_ptr<Texture> titleTex_;
    std::unique_ptr<Sprite> titleSprite_;
    std::shared_ptr<Texture> startTex_;
    std::unique_ptr<Sprite> startSprite_;
    std::shared_ptr<Texture> clearTex_;
    std::unique_ptr<Sprite> clearSprite_;
    std::shared_ptr<Texture> gameOverTex_;
    std::unique_ptr<Sprite> gameOverSprite_;
    std::shared_ptr<Texture> toTitleTex_;
    std::unique_ptr<Sprite> toTitleSprite_;
    /*std::shared_ptr<Texture> startTex_;
    std::unique_ptr<Sprite> startSprite_;*/

    Vector2 titleScale_{};

    float titleAlpha_ = 1.0f;

    float fadeAlpha_ = 0.0f;

    size_t enemyBGM_;
    size_t enemyBGMHandle_ = UNUSED_PLAY_HANDLE;
    size_t bossBGM_;
    size_t bossBGMHandle_ = UNUSED_PLAY_HANDLE;
    size_t titleBGM_;
    size_t titleBGMHandle_ = UNUSED_PLAY_HANDLE;
    size_t selectSE_;

};
