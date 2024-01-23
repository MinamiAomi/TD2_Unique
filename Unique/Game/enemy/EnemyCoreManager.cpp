#include "EnemyCoreManager.h"

EnemyCoreManager* EnemyCoreManager::GetInstance() {
    static EnemyCoreManager instance;
    return &instance;
}
