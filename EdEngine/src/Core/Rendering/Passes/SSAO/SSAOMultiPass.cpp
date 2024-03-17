#include "SSAOMultiPass.h"
#include "SSAOPass.h"
#include "SSAOBlurPass.h"

void SSAOMultiPass::Update(float deltaSeconds)
{
	MultiPassRenderPass<MultiRenderPassParameters, ShaderParameters>::Update(deltaSeconds);

	if (m_Renderer->IsSSAOEnabled())
	{
		for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
		{
			UpdatePass(pass, deltaSeconds);
		}
	}
}

void SSAOMultiPass::CreatePasses()
{
	AddPass<SSAOPass>();
	AddPass<SSAOBlurPass>();
}
