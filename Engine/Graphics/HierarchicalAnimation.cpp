#include "HierarchicalAnimation.h"

#include <Windows.h>
#include <cassert>
#include <stack>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace {

    Animation::Vector3Node::KeyFrame ConvertKeyframe(const aiVectorKey& src, float invDuration) {
        Animation::Vector3Node::KeyFrame keyframe;
        keyframe.value.x = static_cast<float>(src.mValue.x);
        keyframe.value.y = static_cast<float>(src.mValue.y);
        keyframe.value.z = static_cast<float>(src.mValue.z);
        keyframe.timeStamp = std::clamp(static_cast<float>(src.mTime) * invDuration, 0.0f, 1.0f);
        return keyframe;
    }

    Animation::QuaternionNode::KeyFrame ConvertKeyframe(const aiQuatKey& src, float invDuration) {
        Animation::QuaternionNode::KeyFrame keyframe;
        keyframe.value.x = static_cast<float>(src.mValue.x);
        keyframe.value.y = -static_cast<float>(src.mValue.y);
        keyframe.value.z = static_cast<float>(src.mValue.z);
        keyframe.value.w = -static_cast<float>(src.mValue.w);
        keyframe.timeStamp = std::clamp(static_cast<float>(src.mTime) * invDuration, 0.0f, 1.0f);
        return keyframe;
    }

    Matrix4x4 ConvertMatrix(const aiMatrix4x4& m) {
        return {
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4 };
    }

}

std::shared_ptr<HierarchicalAnimation> HierarchicalAnimation::Load(const std::filesystem::path& path) {

    // privateコンストラクタをmake_sharedで呼ぶためのヘルパー
    struct Helper : HierarchicalAnimation {
        Helper() : HierarchicalAnimation() {}
    };
    std::shared_ptr<HierarchicalAnimation> hierarchicalAnimation = std::make_shared<Helper>();

    Assimp::Importer importer;
    int flags = 0;
    // 左手座標系に変換
    flags |= aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFile(path.string(), flags);
    if (!scene) {
        OutputDebugStringA(importer.GetErrorString());
        assert(false);
    }
    assert(scene->mNumAnimations != 0);

    // 初期姿勢行列の読み込み
    std::unordered_map<std::string, Matrix4x4> initialMatrix;
    std::stack<aiNode*> stack;
    stack.push(scene->mRootNode);
    Matrix4x4 glovalInverse = ConvertMatrix(scene->mRootNode->mTransformation).Inverse();

    while (!stack.empty()) {
        auto node = stack.top();
        stack.pop();
        initialMatrix[node->mName.C_Str()] = ConvertMatrix(node->mTransformation);
        if (node->mParent) {
            initialMatrix[node->mName.C_Str()] *= initialMatrix[node->mParent->mName.C_Str()];
        }

        for (size_t i = 0; i < node->mNumChildren; ++i) {
            stack.push(node->mChildren[i]);
        }
    }

    // 0番決め打ち
    aiAnimation* animation = scene->mAnimations[0];
    for (size_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
        auto& srcChannel = animation->mChannels[channelIndex];
        auto& destNode = hierarchicalAnimation->data_[srcChannel->mNodeName.C_Str()];

        float invDuration = 1.0f / static_cast<float>(animation->mDuration);
        std::vector<Animation::Vector3Node::KeyFrame> positionKey;
        std::vector<Animation::QuaternionNode::KeyFrame> rotateKey;
        std::vector<Animation::Vector3Node::KeyFrame> scaleKey;

        for (size_t keyIndex = 0; keyIndex < srcChannel->mNumPositionKeys; ++keyIndex) {
            Animation::Vector3Node::KeyFrame key = ConvertKeyframe(srcChannel->mPositionKeys[keyIndex], invDuration);
            positionKey.emplace_back(key);
        }
        for (size_t keyIndex = 0; keyIndex < srcChannel->mNumRotationKeys; ++keyIndex) {
            Animation::QuaternionNode::KeyFrame key = ConvertKeyframe(srcChannel->mRotationKeys[keyIndex], invDuration);
            rotateKey.emplace_back(key);
        }
        for (size_t keyIndex = 0; keyIndex < srcChannel->mNumScalingKeys; ++keyIndex) {
            Animation::Vector3Node::KeyFrame key = ConvertKeyframe(srcChannel->mScalingKeys[keyIndex], invDuration);
            scaleKey.emplace_back(key);
        }

        destNode.translate = std::move(Animation::Vector3Node(positionKey));
        destNode.rotate = std::move(Animation::QuaternionNode(rotateKey));
        destNode.scale = std::move(Animation::Vector3Node(scaleKey));
        destNode.initialInverseMatrix = initialMatrix[srcChannel->mNodeName.C_Str()].Inverse();
    }

    return hierarchicalAnimation;
}
