#include "BarrierBulletManager.h"

BarrierBulletManager* BarrierBulletManager::GetInstance() {
    static BarrierBulletManager instance;
    return &instance;
}
