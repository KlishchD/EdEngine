#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(SSAOBlurPassParameters, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Blured, Distance, "SSAO.Blured")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, AmbientOcclusion, "SSAO.Base")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(SSAOBlurPassShaderParameters)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, AmbientOcclusion)
	ED_SHADER_PARAMETER(Float2, glm::vec2, PixelSize)

ED_END_SHADER_PARAMETERS_DECLARATION()

class SSAOBlurPass : public RenderPass<SSAOBlurPassParameters, SSAOBlurPassShaderParameters>
{
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;
};