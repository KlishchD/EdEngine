#include "RenderPass.h"

void BaseRenderPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	m_Renderer = graph->GetRenderer();
	m_Context = graph->GetContext();
	m_Graph = graph;
}

void BaseRenderPass::PreUpdate()
{

}

void BaseRenderPass::Update(float deltaSeconds)
{

}
