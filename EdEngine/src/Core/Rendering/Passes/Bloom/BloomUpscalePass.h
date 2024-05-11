#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "BloomMultiPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(BloomUpscalePass, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Upscaled, "Bloom.Intermediate" + std::to_string(BloomMultiPass::MaxBloomDownscalingCount - Parameters.InstanceNumber))

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Downscaled, "Bloom.Intermediate" + std::to_string(BloomMultiPass::MaxBloomDownscalingCount - Parameters.InstanceNumber + 1), Read)
	ED_RENDER_PASS_PARAMETER(uint32_t, DownscaleCount, "Bloom.DownscaleCount", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(BloomUpscalePass)

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
	virtual void Execute() override;

	float GetBloomMixStrength() const;
	void SetBloomMixStrength(float strength);
};