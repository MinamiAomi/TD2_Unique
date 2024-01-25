#pragma once

#include <memory>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "Math/AnimationNode.h"

class HierarchicalAnimation {
public:

    struct NodeSet {
        Animation::Vector3Node translate;
        Animation::QuaternionNode rotate;
        Animation::Vector3Node scale;
    };
    struct Node {
        std::string name;
        Matrix4x4 worldMatrix;
        Node* parent;
    };

    static std::shared_ptr<HierarchicalAnimation> Load(const std::filesystem::path& path);  

    void Update(float duration);
    Matrix4x4 GetNodeWorldMatrix(const std::string& node) const;

private:
    struct Animation {
        std::unordered_map<std::string, NodeSet> nodes;
    };

    std::unordered_map<std::string, Animation> animations;
    

};