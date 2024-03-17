#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(BloomDownscalePassParameters, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Output, Color16, "Bloom.Intermediate" + std::to_string(InstanceNumber))

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, TAAOutput, "TAA.Output")
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, FXAAOutput, "FXAA.Output")
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Scene, "LightBuffer.Combined")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Previous, "Bloom.Intermediate" + std::to_string(std::max<uint32_t>(InstanceNumber - 1, 1)))

	ED_RENDER_PASS_GRAPH_PARAMETER(uint32_t, DownscaleCount, "Bloom.DownscaleCount")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(BloomDownscalePassShaderParameters)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Input)
	ED_SHADER_PARAMETER(Float2, glm::vec2, InPixelSize)
	ED_SHADER_PARAMETER(Float2, glm::vec2, OutPixelSize)

ED_END_SHADER_PARAMETERS_DECLARATION()

class BloomDownscalePass : public RenderPass<BloomDownscalePassParameters, BloomDownscalePassShaderParameters>
{	
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;

	std::shared_ptr<Texture2D> GetTexture() const;
	std::shared_ptr<Framebuffer> GetFramebuffer() const;
};