#include "HitStopManager.h"

HitStopManager* HitStopManager::GetInstance() {
    static HitStopManager instance;
    return &instance;
}
