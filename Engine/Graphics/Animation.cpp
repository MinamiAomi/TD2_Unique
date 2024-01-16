#include "Animation.h"

#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Windows.h>

std::vector<std::shared_ptr<Animation>> Animation::Load(const std::filesystem::path& path) {
    // privateコンストラクタをmake_sharedで呼ぶためのヘルパー
    struct Helper : Animation {
        Helper() : Animation() {}
    };


    auto directory = path.parent_path();

    Assimp::Importer importer;
    int flags = 0;
    const aiScene* scene = importer.ReadFile(path.string(), flags);
    // 読み込めた
    if (!scene) {
        OutputDebugStringA(importer.GetErrorString());
        assert(false);
    }
    // アニメーションがある
    assert(scene->HasAnimations());

    std::vector<std::shared_ptr<Animation>> animations(scene->mNumAnimations);
    
    for (size_t i = 0; i < animations.size(); ++i) {
        animations[i] = std::make_shared<Helper>();
        auto anim = scene->mAnimations[i];
        anim->mChannels;
    }

    return std::vector<std::shared_ptr<Animation>>();
}
