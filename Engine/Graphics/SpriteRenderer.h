#pragma once

#include "Math/MathUtils.h"

#include "Core/RootSignature.h"
#include "Core/PipelineState.h"

class CommandContext;
class ColorBuffer;

class SpriteRenderer {
public:
    void Initialize(const ColorBuffer& colorBuffer);
    void Render(CommandContext& commandContext, float left, float top, float right, float bottom);

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
};