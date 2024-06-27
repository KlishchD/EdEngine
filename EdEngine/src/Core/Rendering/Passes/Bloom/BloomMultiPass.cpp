#include "BloomMultiPass.h"
#include "BloomDownscalePass.h"
#include "BloomUpscalePass.h"

void BloomMultiPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	MultiPassRenderPass<BloomMultiPassParameters>::Initialize(graph);

	m_Parameters.DownscaleCount = 4;
}

void BloomMultiPass::Execute()
{
	MultiPassRenderPass<BloomMultiPassParameters>::Execute();

	if (m_Renderer->IsBloomEnabled())
	{
		// Downscaling
		for (uint32_t i = 0; i < m_Parameters.DownscaleCount; ++i)
		{
			m_Graph->ExecutePass(m_Passes[i]);
		}

		// Upscaling (we are only upscaling to half the resolution of the full scene)
		for (uint32_t i = MaxBloomDownscalingCount * 2 - m_Parameters.DownscaleCount; i < m_Passes.size(); ++i)
		{
			m_Graph->ExecutePass(m_Passes[i]);
		}
	}
}

void BloomMultiPass::CreatePasses()
{
	for (uint32_t i = 0; i < MaxBloomDownscalingCount; ++i)
	{
		AddPass<BloomDownscalePass>();
	}

	for (uint32_t i = 0; i < MaxBloomDownscalingCount - 1; ++i)
	{
		AddPass<BloomUpscalePass>();
	}
}

float BloomMultiPass::GetBloomMixStrength() const
{
	return std::static_pointer_cast<BloomUpscalePass>(*m_Passes.rbegin())->GetBloomMixStrength();
}

void BloomMultiPass::SetBloomMixStrength(float strength)
{
	for (uint32_t i = MaxBloomDownscalingCount; i < m_Passes.size(); ++i)
	{
		std::static_pointer_cast<BloomUpscalePass>(m_Passes[i])->SetBloomMixStrength(strength);
	}
}

uint32_t BloomMultiPass::GetBloomDownscaleCount() const
{
	return m_Parameters.DownscaleCount;
}

void BloomMultiPass::SetBloomDownscaleCount(uint32_t count)
{
	m_Parameters.DownscaleCount = count;
}

