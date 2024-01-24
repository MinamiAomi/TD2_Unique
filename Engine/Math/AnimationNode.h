#pragma once

#include <vector>

#include "MathUtils.h"

namespace Animation {

    template<class T>
    class Node {
    public:
        using Value = T;
        struct KeyFrame {
            Value value;
            float timeStamp;
        };

        Node() = default;
        explicit Node(const KeyFrame& keyFrames) { keyFrames_.emplace_back(keyFrames); }
        explicit Node(const std::vector<KeyFrame>& keyFrames) { keyFrames_ = keyFrames; }
        explicit Node(std::vector<KeyFrame>&& keyFrames) { keyFrames_ = std::move(keyFrames); }

        virtual ~Node() {}
        virtual Value GetInterpolatedValue(float animationTime) = 0;

        void AddKeyFrame(const Value& value, float timeStamp) { keyFrames_.emplace_back(value, timeStamp); }
        void AddKeyFrames(const std::vector<KeyFrame>& keyFrames) { keyFrames_.insert(keyFrames_.end(), keyFrames.begin(), keyFrames.end()); }

        size_t GetKeyFrameIndex(float animationTime) {
            for (size_t index = 0; index < keyFrames_.size() - 1; ++index) {
                if (animationTime < keyFrames_[index + 1].timeStamp) {
                    return index;
                }
            }
            return keyFrames_.size();
        }

        size_t GetNumKeyFrames() const { return keyFrames_.size(); }

        float GetScaleFactor(float prevTimeStamp, float nextTimeStamp, float animationTime) {
            float midWayLength = animationTime - prevTimeStamp;
            float framesDiff = nextTimeStamp - prevTimeStamp;
            return midWayLength / framesDiff;
        }

    protected:
        std::vector<KeyFrame> keyFrames_;
    };

    class FloatNode :
        public Node<float> {
    public:
        FloatNode() = default;
        FloatNode(const KeyFrame& keyFrame) : Node(keyFrame) {}
        FloatNode(const std::vector<KeyFrame>& keyFrames) : Node(keyFrames) {}
        FloatNode(std::vector<KeyFrame>&& keyFrames) : Node(std::move(keyFrames)) {}
        float GetInterpolatedValue(float animationTime) override;
    };

    class Vector3Node :
        public Node<Vector3> {
    public:
        Vector3Node() = default;
        Vector3Node(const KeyFrame& keyFrame) : Node(keyFrame) {}
        Vector3Node(const std::vector<KeyFrame>& keyFrames) : Node(keyFrames) {}
        Vector3Node(std::vector<KeyFrame>&& keyFrames) : Node(std::move(keyFrames)) {}
        Vector3 GetInterpolatedValue(float animationTime) override;
    };

    class QuaternionNode :
        public Node<Quaternion> {
    public:
        QuaternionNode() = default;
        QuaternionNode(const KeyFrame& keyFrame) : Node(keyFrame) {}
        QuaternionNode(const std::vector<KeyFrame>& keyFrames) : Node(keyFrames) {}
        QuaternionNode(std::vector<KeyFrame>&& keyFrames) : Node(std::move(keyFrames)) {}
        Quaternion GetInterpolatedValue(float animationTime) override;
    };

}