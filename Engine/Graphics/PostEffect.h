#pragma once

#include "Core/RootSignature.h"
#include "Core/PipelineState.h"
#include "Core/ColorBuffer.h"

class CommandContext;

class PostEffect {
public:

    void Initialize(const ColorBuffer& target);
    void Render(CommandContext& commandContext, ColorBuffer& texture, ColorBuffer& shadow, ColorBuffer& reflection);
    void Render(CommandContext& commandContext, ColorBuffer& texture);
    void RenderAddTexture(CommandContext& commandContext, ColorBuffer& texture);
    void RenderMultiplyTexture(CommandContext& commandContext, ColorBuffer& texture);


private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    PipelineState pipelineStateOther_;
    PipelineState pipelineStateAdd_;
    PipelineState pipelineStateMultiply_;

    ColorBuffer* sourceTexture_;
};