#pragma once

#include <memory>
#include <filesystem>
#include <string>
#include <vector>

#include "Math/AnimationNode.h"

class HierarchicalAnimation {
public:

    static std::vector<std::shared_ptr<HierarchicalAnimation>> Load(const std::filesystem::path& path);
    
private:

};