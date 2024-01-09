#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Core/GPUBuffer.h"
#include "Math/MathUtils.h"
#include "Material.h"

class CommandContext;

struct Mesh {
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 texcood;
    };

    using Index = uint32_t;

    StructuredBuffer vertexBuffer;
    StructuredBuffer indexBuffer;

    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    std::shared_ptr<Material> material;

    void CreateBuffers(CommandContext& commandContext);
};