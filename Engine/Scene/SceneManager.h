#pragma once

#include <memory>

class BaseScene;

class SceneManager {
public:
    static SceneManager* GetInstance();

    void Update();

    template<class T>
    void ChangeScene() {
        static_assert(std::is_base_of<BaseScene, T>::value, "BaseSceneを継承していません。");
        nextScene_ = std::make_unique<T>();
    }

private:
    SceneManager();
    ~SceneManager();
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    std::unique_ptr<BaseScene> currentScene_;
    std::unique_ptr<BaseScene> nextScene_;
};