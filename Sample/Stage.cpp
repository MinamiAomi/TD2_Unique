#include "Stage.h"

struct FloorInitData {
    Vector3 base;
    float rotate;
    float movement;
    uint32_t moveCycle;
};

static const FloorInitData stageInits[] = {
    { {  0.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { {  0.0f, -1.0f, 10.0f }, 0.0f,         0.0f,   0  },
    { {  0.0f, -1.0f, 15.0f }, 0.0f,         0.0f,   0  },
    { {  0.0f, -1.0f, 20.0f }, 0.0f,         0.0f,   0  },
    { { 10.0f, -1.0f, 20.0f }, Math::HalfPi, 5.0f, 300  },
    { { 20.0f, -1.0f, 20.0f }, 0.0f,         0.0f,   0  },

    { { 20.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { { 25.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { { 30.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { { 20.0f, -1.0f,  5.0f }, 0.0f,         0.0f,   0  },
    { { 25.0f, -1.0f,  5.0f }, 0.0f,         0.0f,   0  },
    { { 30.0f, -1.0f,  5.0f }, 0.0f,         0.0f,   0  },
    { { 20.0f, -1.0f,  10.0f }, 0.0f,         0.0f,   0  },
    { { 25.0f, -1.0f,  10.0f }, 0.0f,         0.0f,   0  },
    { { 30.0f, -1.0f,  10.0f }, 0.0f,         0.0f,   0  },
    { { 20.0f, -1.0f,  -5.0f }, 0.0f,         0.0f,   0 },
    { { 25.0f, -1.0f,  -5.0f }, 0.0f,         0.0f,   0 },
    { { 30.0f, -1.0f,  -5.0f }, 0.0f,         0.0f,   0 },
    { {  5.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { { 10.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { { 15.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { {-10.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
    { { -5.0f, -1.0f,  0.0f }, 0.0f,         0.0f,   0  },
};

struct EnemyInitData {
    Vector3 pos;
};

static const EnemyInitData enemyInits[] = {
    { {  0.0f, 0.0f, 20.0f } },
    { { 20.0f, 0.0f,  0.0f } },
    { { 30.0f, 0.0f,  0.0f } },
    { { 20.0f, 0.0f, 10.0f } },
    { { 30.0f, 0.0f, 10.0f } },
};

void Stage::Initialize() {

    floors_.resize(_countof(stageInits));

    for (uint32_t i = 0; auto & floor : floors_) {
        floor = std::make_shared<Floor>();
        floor->Initialize(stageInits[i].base, stageInits[i].rotate, stageInits[i].movement, stageInits[i].moveCycle);
        i++;
    }

    enemies_.resize(_countof(enemyInits));
    for (uint32_t i = 0; auto & enemy : enemies_) {
        enemy = std::make_shared<Enemy>();
        enemy->Initialize(enemyInits[i].pos);
        i++;
    }

    const uint32_t goalFloorIndex = 5;
    goal_ = std::make_shared<Goal>();
    goal_->transform.SetParent(&floors_[goalFloorIndex]->transform);
    goal_->transform.translate = { 0.0f,1.0f,0.0f };
    goal_->Initialize();
}

void Stage::Update() {
    for (auto& enemy : enemies_) {
        enemy->Update();
    }

    for (auto& floor : floors_) {
        floor->Update();
    }
    goal_->Update();
}

void Stage::Restart() {
    enemies_.resize(_countof(enemyInits));
    for (uint32_t i = 0; auto & enemy : enemies_) {
        enemy = std::make_shared<Enemy>();
        enemy->Initialize(enemyInits[i].pos);
        i++;
    }
}
