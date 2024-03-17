#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(AmbientPassParameters, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Diffuse,  Color16, "LightBuffer.Diffuse")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Specular, Color16, "LightBuffer.Specular")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Combined, Color16, "LightBuffer.Combined")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Albedo, "GBuffer.Albedo")
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, AmbientOcclusion, "SSAO.Base")
	
ED_END_RENDER_PASS_PARAMETERS_DECLARATION()
	
ED_BEGIN_SHADER_PARAMETERS_DECLARATION(AmbientPassShaderParameters)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Albedo)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, AmbientOcclusion)

ED_END_SHADER_PARAMETERS_DECLARATION()

class AmbientPass : public RenderPass<AmbientPassParameters, AmbientPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;
};