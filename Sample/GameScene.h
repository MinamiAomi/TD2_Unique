#pragma once
#include "Scene/BaseScene.h"

#include <memory>

#include "Player.h"
#include "FollowCamera.h"
#include "Ground.h"
#include "Skydome.h"
#include "Stage.h"
#include "LookOn.h"

class GameScene :
    public BaseScene {
public:

    void OnInitialize() override;
    void OnUpdate() override;
    void OnFinalize() override;

private:
    std::shared_ptr<Player> player_;
    std::shared_ptr<FollowCamera> followCamera_;
    //std::shared_ptr<Ground> ground_;
    std::shared_ptr<Skydome> skydome_;
    std::shared_ptr<Stage> stage_;
    std::shared_ptr<LookOn> lookOn_;
};
