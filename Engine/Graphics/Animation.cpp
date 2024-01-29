#include "Animation.h"

#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stack>

namespace {

    Animation::Vector3Node::KeyFrame ConvertKeyframe(aiVectorKey src, float invDuration) {
        Animation::Vector3Node::KeyFrame keyframe;
        keyframe.value.x = static_cast<float>(src.mValue.x);
        keyframe.value.y = static_cast<float>(src.mValue.y);
        keyframe.value.z = static_cast<float>(src.mValue.z);
        keyframe.timeStamp = std::clamp(static_cast<float>(src.mTime) * invDuration, 0.0f, 1.0f);
        return keyframe;
    }

    Animation::QuaternionNode::KeyFrame ConvertKeyframe(aiQuatKey src, float invDuration) {
        Animation::QuaternionNode::KeyFrame keyframe;
        keyframe.value.x = static_cast<float>(src.mValue.x);
        keyframe.value.y = static_cast<float>(src.mValue.y);
        keyframe.value.z = static_cast<float>(src.mValue.z);
        keyframe.value.w = static_cast<float>(src.mValue.w);
        keyframe.timeStamp = std::clamp(static_cast<float>(src.mTime) * invDuration, 0.0f, 1.0f);
        return keyframe;
    }

}

std::shared_ptr<HierarchicalAnimation> HierarchicalAnimation::Load(const std::filesystem::path& path) {

    std::shared_ptr<HierarchicalAnimation> hierarchicalAnimation = std::make_shared<HierarchicalAnimation>();

    Assimp::Importer importer;
    int flags = 0;
    // 左手座標系に変換
    flags |= aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFile(path.string(), flags);


    for (size_t animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex) {
        aiAnimation* animation = scene->mAnimations[animIndex];
        for (size_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
            auto& srcChannel = animation->mChannels[channelIndex];
            auto& destNode = hierarchicalAnimation->animations_[animation->mName.C_Str()].nodes[srcChannel->mNodeName.C_Str()];

            float invDuration = 1.0f / static_cast<float>(animation->mDuration);
            std::vector<Animation::Vector3Node::KeyFrame> positionKey;
            std::vector<Animation::QuaternionNode::KeyFrame> rotateKey;
            std::vector<Animation::Vector3Node::KeyFrame> scaleKey;

            for (size_t keyIndex = 0; keyIndex < srcChannel->mNumPositionKeys; ++keyIndex) {
                positionKey.emplace_back(ConvertKeyframe(srcChannel->mPositionKeys[keyIndex], invDuration));
            }
            for (size_t keyIndex = 0; keyIndex < srcChannel->mNumRotationKeys; ++keyIndex) {
                rotateKey.emplace_back(ConvertKeyframe(srcChannel->mRotationKeys[keyIndex], invDuration));
            }
            for (size_t keyIndex = 0; keyIndex < srcChannel->mNumScalingKeys; ++keyIndex) {
                scaleKey.emplace_back(ConvertKeyframe(srcChannel->mScalingKeys[keyIndex], invDuration));
            }

            destNode.translate = std::move(Animation::Vector3Node(positionKey));
            destNode.rotate = std::move(Animation::QuaternionNode(rotateKey));
            destNode.scale = std::move(Animation::Vector3Node(scaleKey));
        }
    }




    return std::shared_ptr<HierarchicalAnimation>();
}

void HierarchicalAnimation::Update(float duration)
{
}

Matrix4x4 HierarchicalAnimation::GetNodeWorldMatrix(const std::string& node) const {
    return Matrix4x4();
}
