#include "EdRendererPrivate.h"

// #include "RenderPass.h"
// 
// void BaseMultiPassRenderPass::PostInitialization()
// {
// 	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
// 	{
// 		pass->Initialize(m_Graph);
// 	}
// }

void RenderPass::Initialize(RenderGraph* graph)
{
    m_Graph = graph;
    m_Renderer = &Renderer::Get();
    m_Context = &RenderingContext::Get();
    m_List = m_Context->CreateCommandList(GetCommandQueueType(), m_Name);
}
