#include "EdRendererPrivate.h"

// #include "BloomMultiPass.h"
// #include "BloomDownscalePass.h"
// #include "BloomUpscalePass.h"
// 
// void BloomMultiPass::Initialize(std::shared_ptr<RenderGraph> graph)
// {
// 	MultiPassRenderPass<BloomMultiPassParameters>::Initialize(graph);
// 
// 	m_Parameters.DownscaleCount = 4;
// }
// 
// void BloomMultiPass::Execute()
// {
// 	MultiPassRenderPass<BloomMultiPassParameters>::Execute();
// 
// 	if (m_Renderer->IsBloomEnabled())
// 	{
// 		// Downscaling
// 		for (u32 i = 0; i < m_Parameters.DownscaleCount; ++i)
// 		{
// 			m_Graph->ExecutePass(m_Passes[i]);
// 		}
// 
// 		// Upscaling (we are only upscaling to half the resolution of the full scene)
// 		for (u32 i = MaxBloomDownscalingCount * 2 - m_Parameters.DownscaleCount; i < m_Passes.size(); ++i)
// 		{
// 			m_Graph->ExecutePass(m_Passes[i]);
// 		}
// 	}
// }
// 
// void BloomMultiPass::CreatePasses()
// {
// 	for (u32 i = 0; i < MaxBloomDownscalingCount; ++i)
// 	{
// 		AddPass<BloomDownscalePass>();
// 	}
// 
// 	for (u32 i = 0; i < MaxBloomDownscalingCount - 1; ++i)
// 	{
// 		AddPass<BloomUpscalePass>();
// 	}
// }
// 
// f32 BloomMultiPass::GetBloomMixStrength() const
// {
// 	return std::static_pointer_cast<BloomUpscalePass>(*m_Passes.rbegin())->GetBloomMixStrength();
// }
// 
// void BloomMultiPass::SetBloomMixStrength(f32 strength)
// {
// 	for (u32 i = MaxBloomDownscalingCount; i < m_Passes.size(); ++i)
// 	{
// 		std::static_pointer_cast<BloomUpscalePass>(m_Passes[i])->SetBloomMixStrength(strength);
// 	}
// }
// 
// u32 BloomMultiPass::GetBloomDownscaleCount() const
// {
// 	return m_Parameters.DownscaleCount;
// }
// 
// void BloomMultiPass::SetBloomDownscaleCount(u32 count)
// {
// 	m_Parameters.DownscaleCount = count;
// }
// 
