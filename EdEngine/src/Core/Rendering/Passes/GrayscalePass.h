#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Rendering/Textures/Texture2D.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(GrayscalePass, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Color, "Resolution.Color")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(GrayscalePass)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Color)

ED_END_SHADER_PARAMETERS_DECLARATION()

class GrayscalePass : public RenderPass<GrayscalePassParameters, GrayscalePassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;
};
