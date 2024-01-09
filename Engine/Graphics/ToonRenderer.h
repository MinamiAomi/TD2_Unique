#pragma once

#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Core/TextureResource.h"

class CommandContext;
class ColorBuffer;
class DepthBuffer;
class Camera;

struct ToonRootIndex {
    enum Index {
        Scene = 0,
        Instance,
        Material,
        Texture,
        Sampler,
        DirectionalLight,

        NumParameters
    };
};

class ToonRenderer {
public:

    void Initialize(const ColorBuffer& colorBuffer, const DepthBuffer& depthBuffer);
    void Render(CommandContext& commandContext, const Camera& camera);

private:
    void InitializeRootSignature();
    void InitializeOutlinePass(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    void InitializeToonPass(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);

    RootSignature rootSignature_;

    PipelineState outlinePipelineState_;    
    PipelineState toonPipelineState_;
};