#include "DirectionalLightMultiPass.h"
#include "DirectionalLightShadowPass.h"
#include "DirectionalLightShadingPass.h"

void DirectionalLightMultiPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	MultiPassRenderPass<DirectionalLightMultiPassParameters, ShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Directional Light MultiPass";
}

void DirectionalLightMultiPass::Update(float deltaSeconds)
{
	MultiPassRenderPass<DirectionalLightMultiPassParameters, ShaderParameters>::Update(deltaSeconds);

	for (const std::shared_ptr<DirectionalLightComponent>& light : m_Parameters.Lights.Get())
	{
		m_Parameters.Light = light;

		if (light->GetIntensity() != 0.0f)
		{
			for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
			{
				UpdatePass(pass, deltaSeconds);
			}
		}
	}
}

void DirectionalLightMultiPass::CreatePasses()
{
	AddPass<DirectionalLightShadowPass>();
	AddPass<DirectionalLightShadingRenderPass>();
}
