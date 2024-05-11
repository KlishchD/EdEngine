#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(ResolutionPass, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Color, Color16, "Resolution.Color")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, TAAOutput,  "TAA.Output",           Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, FXAAOutput, "FXAA.Output",          Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Light,      "LightBuffer.Combined", Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Bloom,      "Bloom.Intermediate1",  Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(ResolutionPass)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Light)
	ED_SHADER_PARAMETER(Float, float, Gamma)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Bloom)
	ED_SHADER_PARAMETER(Bool, bool, IsBloomEnabled)
	ED_SHADER_PARAMETER(Float, float, BloomStrength)
	ED_SHADER_PARAMETER(Float, float, BloomIntensity)

ED_END_SHADER_PARAMETERS_DECLARATION()

class ResolutionPass : public RenderPass<ResolutionPassParameters, ResolutionPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;

	void SetGamma(float gamma);
	float GetGamma() const;

	void SetBloomStrength(float strength);
	float GetBloomStrength() const;

	void SetBloomIntensity(float intensity);
	float GetBloomIntensity() const;
};