#pragma once

#include <memory>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "Math/AnimationNode.h"

class HierarchicalAnimation {
public:
    struct Node {
        Animation::Vector3Node translate;
        Animation::QuaternionNode rotate;
        Animation::Vector3Node scale;
        Matrix4x4 initialInverseMatrix;
    };

    static std::shared_ptr<HierarchicalAnimation> Load(const std::filesystem::path& path);  

    const Node& GetNode(const std::string& nodeName) const { return data_.at(nodeName); }

private:
    HierarchicalAnimation() = default;
    ~HierarchicalAnimation() = default;

    std::unordered_map<std::string, Node> data_;
    std::unordered_map<std::string, Matrix4x4> mat_;
};