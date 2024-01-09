#pragma once

#include <map>
#include <string>

#include "../Core/ColorBuffer.h"
#include "../Core/RootSignature.h"
#include "../../Math/Camera.h"

#include "StateObject.h"
#include "TLAS.h"
#include "ShaderTable.h"

class CommandContext;
class DirectionalLight;

class RTShadowRenderer {
public:
    void Create(uint32_t width, uint32_t height);

    void Render(CommandContext& commandContext, const Camera& camera, const DirectionalLight& sunLight);

    ColorBuffer& GetShadow() { return shadowBuffer_; }

private:
    void CreateRootSignature();
    void CreateStateObject();
    void CreateShaderTables();
    bool BuildScene(CommandContext& commandContext);

    StateObject stateObject_;
    RootSignature globalRootSignature_;
    RootSignature hitGroupLocalRootSignature_;
    
    TLAS reciveShadowTlas_;
    TLAS castShadowTlas_;

    ShaderTable rayGenerationShaderTable_;
    ShaderTable hitGroupShaderTable_;
    ShaderTable missShaderTable_;

    std::map<std::wstring, void*> identifierMap_;

    ColorBuffer shadowBuffer_;
};