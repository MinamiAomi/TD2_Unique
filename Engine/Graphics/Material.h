#pragma once

#include <memory>
#include <string>

#include "Math/MathUtils.h"
#include "Core/TextureResource.h"
#include "Core/UploadBuffer.h"

struct Material {
    Vector3 diffuse{ 0.8f, 0.8f, 0.8f };
    Vector3 specular{ 0.5f, 0.5f, 0.5f };
    Vector3 ambient{ 0.0f, 0.0f, 0.0f };
    float shininess{ 10.0f };
    std::shared_ptr<TextureResource> diffuseMap;
};

struct PBRMaterial {
    Vector3 baseColor;
    float metallic;
    float roughness;
    std::shared_ptr<TextureResource> baseColorMap;
    std::shared_ptr<TextureResource> metallicRoughnessMap;
    std::shared_ptr<TextureResource> noramlMap;
};