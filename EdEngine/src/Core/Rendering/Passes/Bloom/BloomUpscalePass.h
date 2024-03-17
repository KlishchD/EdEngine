#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "BloomMultiPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(BloomUpscalePassParameters, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Upscaled, "Bloom.Intermediate" + std::to_string(BloomMultiPass::MaxBloomDownscalingCount - InstanceNumber))

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Downscaled, "Bloom.Intermediate" + std::to_string(BloomMultiPass::MaxBloomDownscalingCount - InstanceNumber + 1))
	ED_RENDER_PASS_GRAPH_PARAMETER(uint32_t, DownscaleCount, "Bloom.DownscaleCount")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(BloomUpscalePassShaderParameters)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Downscaled)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Upscaled)

	ED_SHADER_PARAMETER(Float2, glm::vec2, UpscaledPixelSize)
	ED_SHADER_PARAMETER(Float2, glm::vec2, DownscaledPixelSize)

	ED_SHADER_PARAMETER(Float, float, MixStrength)

ED_END_SHADER_PARAMETERS_DECLARATION()

class BloomUpscalePass : public RenderPass<BloomUpscalePassParameters, BloomUpscalePassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;

	float GetBloomMixStrength() const;
	void SetBloomMixStrength(float strength);
};