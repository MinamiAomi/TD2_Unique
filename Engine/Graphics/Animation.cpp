#include "Animation.h"

#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stack>

std::vector<std::shared_ptr<HierarchicalAnimation>> HierarchicalAnimation::Load(const std::filesystem::path& path) {

    Assimp::Importer importer;
    int flags = 0;
    // 左手座標系に変換
    flags |= aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFile(path.string(), flags);

    std::vector<aiAnimation*> animations(scene->mNumAnimations);
    for (int i = 0; i < animations.size(); ++i) {
        animations[i] = scene->mAnimations[i];
    }


    std::vector<aiNode*> nodes;

    std::stack<aiNode*> stack;
    stack.push(scene->mRootNode);

    while (!stack.empty()) {
        aiNode* node = stack.top();
        stack.pop();
        
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            nodes.emplace_back(node->mChildren[i]);
            stack.push(node->mChildren[i]);
        }
    }



    return std::vector<std::shared_ptr<HierarchicalAnimation>>();
}
