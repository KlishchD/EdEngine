#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(FXAAPass, Compute)

	ED_RENDER_PASS_DECLARE_RESOURCE(Texture2D, Output, "FXAA.Output")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, LightCombined, "LightBuffer.Combined", Read)

ED_END_SHADER_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(FXAAPass)
	
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Input)
	ED_SHADER_PARAMETER_PTR(Image, Texture2D, Output)

	ED_SHADER_PARAMETER(Float, float, ContrastThreshold)
	ED_SHADER_PARAMETER(Float, float, RelativeThreshold)
	ED_SHADER_PARAMETER(Float, float, SubpixelBlending)

	ED_SHADER_PARAMETER(Float2, glm::vec2, PixelSize)

ED_END_SHADER_PARAMETERS_DECLARATION()

class FXAAPass : public RenderPass<FXAAPassParameters, FXAAPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph);
	virtual void Execute();

	void SetContrastThreshold(float threshold);
	float GetContrastThreshold() const;

	void SetRelativeThreshold(float threshold);
	float GetRelativeThreshold() const;

	void SetSubpixelBlending(float scale);
	float GetSubpixelBlending() const;
};