#include "DirectionalLightMultiPass.h"
#include "DirectionalLightShadowPass.h"
#include "DirectionalLightShadingPass.h"

void DirectionalLightMultiPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	MultiPassRenderPass<DirectionalLightMultiPassParameters>::Initialize(graph);

	m_Parameters.Name = "Directional Light MultiPass";
}

void DirectionalLightMultiPass::Execute()
{
	MultiPassRenderPass<DirectionalLightMultiPassParameters>::Execute();

	for (const std::shared_ptr<DirectionalLightComponent>& light : m_Parameters.Lights.Get())
	{
		m_Parameters.Light = light;

		if (light->GetIntensity() != 0.0f)
		{
			for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
			{
				m_Graph->ExecutePass(pass);
			}
		}
	}
}

void DirectionalLightMultiPass::CreatePasses()
{
	AddPass<DirectionalLightShadowPass>();
	AddPass<DirectionalLightShadingRenderPass>();
}
