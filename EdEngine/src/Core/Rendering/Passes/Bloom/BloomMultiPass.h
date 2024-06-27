#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(BloomMultiPass, Multi)

	ED_RENDER_PASS_DECLARE_PARAMETER(uint32_t, DownscaleCount, "Bloom.DownscaleCount")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

class BloomMultiPass : public MultiPassRenderPass<BloomMultiPassParameters>
{
public:
	static const uint32_t MaxBloomDownscalingCount = 8;

public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph);
	virtual void Execute() override;

	virtual void CreatePasses() override;

	float GetBloomMixStrength() const;
	void SetBloomMixStrength(float strength);
	
	uint32_t GetBloomDownscaleCount() const;
	void SetBloomDownscaleCount(uint32_t count);

protected:
	std::shared_ptr<Texture2D> m_Output;
};