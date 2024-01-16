#pragma once

#include <vector>
#include <memory>
#include <filesystem>

#include "Math/MathUtils.h"

class Animation {
public:
    static std::vector<std::shared_ptr<Animation>> Load(const std::filesystem::path& path);


private:
    Animation() = default;
    ~Animation() = default;

    std::string name_;
};