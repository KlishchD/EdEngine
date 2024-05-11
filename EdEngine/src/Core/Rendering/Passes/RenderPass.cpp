#include "RenderPass.h"

void BaseRenderPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	m_Renderer = graph->GetRenderer();
	m_Context = graph->GetContext();
	m_Graph = graph;
}

void BaseRenderPass::PostInitialization()
{

}

void BaseRenderPass::PreExecute()
{

}

void BaseRenderPass::Execute()
{

}

void BaseMultiPassRenderPass::PostInitialization()
{
	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
	{
		pass->Initialize(m_Graph);
	}
}
