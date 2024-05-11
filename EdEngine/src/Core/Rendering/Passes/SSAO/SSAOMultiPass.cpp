#include "SSAOMultiPass.h"
#include "SSAOPass.h"
#include "SSAOBlurPass.h"

void SSAOMultiPass::Execute()
{
	MultiPassRenderPass<MultiRenderPassParameters, ShaderParameters>::Execute();

	if (m_Renderer->IsSSAOEnabled())
	{
		for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
		{
			m_Graph->ExecutePass(pass);
		}
	}
}

void SSAOMultiPass::CreatePasses()
{
	AddPass<SSAOBasePass>();
	AddPass<SSAOBlurPass>();
}
