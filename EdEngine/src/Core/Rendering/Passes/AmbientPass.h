#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(AmbientPass, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Diffuse,  Color16, "LightBuffer.Diffuse")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Specular, Color16, "LightBuffer.Specular")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Combined, Color16, "LightBuffer.Combined")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Albedo,           "GBuffer.Albedo", Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, AmbientOcclusion, "SSAO.Base",      Read)
	
ED_END_RENDER_PASS_PARAMETERS_DECLARATION()
	
ED_BEGIN_SHADER_PARAMETERS_DECLARATION(AmbientPass)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Albedo)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, AmbientOcclusion)

ED_END_SHADER_PARAMETERS_DECLARATION()

class AmbientPass : public RenderPass<AmbientPassParameters, AmbientPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;
};