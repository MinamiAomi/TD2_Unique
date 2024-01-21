#include "SmallEnemyManager.h"

SmallEnemyManager* SmallEnemyManager::GetInstance() {
    static SmallEnemyManager instance;
    return &instance;
}
