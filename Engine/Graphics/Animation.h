#pragma once

#include <memory>
#include <filesystem>
#include <string>

#include "Math/AnimationNode.h"

class HierarchicalAnimation {
    static std::shared_ptr<HierarchicalAnimation> Load(const std::filesystem::path&);
    
private:

};