#include "BulletManager.h"

BulletManager* BulletManager::GetInstance() {
    static BulletManager instance;
    return &instance;
}
