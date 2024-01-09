#pragma once
#include "../Core/GPUResource.h"

#include <vector>

#include "../Mesh.h"

class StructuredBuffer;
class CommandContext;

class BLAS : 
    public GPUResource {
public:
    void Create(const std::wstring& name, CommandContext& commandContext, const std::vector<Mesh>& meshes);
};