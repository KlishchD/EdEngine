#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(EmissionPass, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Diffuse,  "LightBuffer.Diffuse")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Specular, "LightBuffer.Specular")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Combined, "LightBuffer.Combined")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Albedo,           "GBuffer.Albedo",           Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, RoughnessMetalic, "GBuffer.RoughnessMetalic", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(EmissionPass)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Albedo)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, RoughnessMetalic)

	ED_SHADER_PARAMETER(Float2, glm::vec2, PixelSize)

ED_END_SHADER_PARAMETERS_DECLARATION()

class EmissionPass : public RenderPass<EmissionPassParameters, EmissionPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;
};