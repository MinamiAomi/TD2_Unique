#include "SceneManager.h"

#include "BaseScene.h"

SceneManager* SceneManager::GetInstance() {
    static SceneManager instance;
    return &instance;
}

void SceneManager::Update() {
    if (nextScene_) {
        if (currentScene_) { currentScene_->OnFinalize(); }
        currentScene_ = std::move(nextScene_);
        nextScene_ = nullptr;
        currentScene_->OnInitialize();
    }

    if (currentScene_) {
        currentScene_->OnUpdate();
    }
}

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
    if (currentScene_) { currentScene_->OnFinalize(); }
    if (nextScene_) { nextScene_->OnFinalize(); }
}

