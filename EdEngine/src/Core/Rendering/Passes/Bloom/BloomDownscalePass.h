#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(BloomDownscalePass, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Output, Color16, "Bloom.Intermediate" + std::to_string(Parameters.InstanceNumber))

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, TAAOutput, "TAA.Output", Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, FXAAOutput, "FXAA.Output", Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Scene, "LightBuffer.Combined", Read)

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Previous, "Bloom.Intermediate" + std::to_string(std::max<int32_t>(Parameters.InstanceNumber - 1, 1)), Read)

	ED_RENDER_PASS_PARAMETER(uint32_t, DownscaleCount, "Bloom.DownscaleCount", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(BloomDownscalePass)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Input)
	ED_SHADER_PARAMETER(Float2, glm::vec2, InPixelSize)
	ED_SHADER_PARAMETER(Float2, glm::vec2, OutPixelSize)

ED_END_SHADER_PARAMETERS_DECLARATION()

class BloomDownscalePass : public RenderPass<BloomDownscalePassParameters, BloomDownscalePassShaderParameters>
{	
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;

	std::shared_ptr<Texture2D> GetTexture() const;
	std::shared_ptr<Framebuffer> GetFramebuffer() const;
};