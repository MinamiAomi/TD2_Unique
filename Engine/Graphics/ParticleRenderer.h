#pragma once

#include "Core/RootSignature.h"
#include "Core/PipelineState.h"

#include "Math/Camera.h"

class ColorBuffer;
class DepthBuffer;
class CommandContext;

class ParticleRenderer {
public:
    static const uint32_t kMaxNumParticles = 1024;
   
    void Initialize(const ColorBuffer& colorBuffer, const DepthBuffer& depthBuffer);
    void Render(CommandContext& commandContext, const Camera& camera);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
};