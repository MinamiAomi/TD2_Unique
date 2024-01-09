#pragma once

#include <memory>
#include <vector>

#include "Enemy.h"
#include "Floor.h"
#include "Goal.h"

class Stage {
public:
    void Initialize();
    void Update();

    void Restart();
    const std::vector<std::shared_ptr<Enemy>>& GetEnemies() const { return enemies_; }

private:
    std::shared_ptr<Goal> goal_;
    std::vector<std::shared_ptr<Enemy>> enemies_;
    std::vector<std::shared_ptr<Floor>> floors_;
};