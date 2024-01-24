#include "AnimationNode.h"

#include <cassert>

namespace Animation {

    float FloatNode::GetInterpolatedValue(float animationTime) {
        // キーフレームは一つ以上存在する
        assert(!keyFrames_.empty());
        // 一つの場合はそれ
        if (keyFrames_.size() == 1) { return keyFrames_[0].value; }
        // 1を超える場合最後のキーフレームの値
        if (animationTime >= 1.0f) { return keyFrames_.back().value; }

        size_t prevIndex = GetKeyFrameIndex(animationTime);
        size_t nextIndex = prevIndex + 1;
        float scaleFactor = GetScaleFactor(keyFrames_[prevIndex].timeStamp, keyFrames_[nextIndex].timeStamp, animationTime);
        return Math::Lerp(scaleFactor, keyFrames_[prevIndex].value, keyFrames_[nextIndex].value);
    }

    Vector3 Vector3Node::GetInterpolatedValue(float animationTime) {
        // キーフレームは一つ以上存在する
        assert(!keyFrames_.empty());
        // 一つの場合はそれ
        if (keyFrames_.size() == 1) { return keyFrames_[0].value; }
        // 1を超える場合最後のキーフレームの値
        if (animationTime >= 1.0f) { return keyFrames_.back().value; }

        size_t prevIndex = GetKeyFrameIndex(animationTime);
        size_t nextIndex = prevIndex + 1;
        float scaleFactor = GetScaleFactor(keyFrames_[prevIndex].timeStamp, keyFrames_[nextIndex].timeStamp, animationTime);
        return Vector3::Lerp(scaleFactor, keyFrames_[prevIndex].value, keyFrames_[nextIndex].value);
    }

    Quaternion QuaternionNode::GetInterpolatedValue(float animationTime) {
        // キーフレームは一つ以上存在する
        assert(!keyFrames_.empty());
        // 一つの場合はそれ
        if (keyFrames_.size() == 1) { return keyFrames_[0].value; }
        // 1を超える場合最後のキーフレームの値
        if (animationTime >= 1.0f) { return keyFrames_.back().value; }

        size_t prevIndex = GetKeyFrameIndex(animationTime);
        size_t nextIndex = prevIndex + 1;
        float scaleFactor = GetScaleFactor(keyFrames_[prevIndex].timeStamp, keyFrames_[nextIndex].timeStamp, animationTime);
        return Quaternion::Slerp(scaleFactor, keyFrames_[prevIndex].value, keyFrames_[nextIndex].value).Normalized();
    }

}