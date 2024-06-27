#include "PointLightMultiPass.h"
#include "PointLightShadowPass.h"
#include "PointLightShadingPass.h"
#include "PointLightWireframePass.h"
#include "Helpers/GeometryBuilder.h"

void PointLightMultiPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	MultiPassRenderPass<PointLightMultiPassParameters>::Initialize(graph);

	m_Parameters.Name = "Point Light MultiPass";

	{
		auto [vertices, indices] = GeometryBuilder::MakeSphere(1, PointLightMeshSectorsCount, PointLightMeshStackCount);

		m_Parameters.LightMeshVertices = std::move(vertices);
		m_Parameters.LightMeshIndices = std::move(indices);
	}
}

void PointLightMultiPass::Execute()
{
	MultiPassRenderPass<PointLightMultiPassParameters>::Execute();

	for (const std::shared_ptr<PointLightComponent>& light : m_Parameters.Lights.Get())
	{
		m_Parameters.Light = light;

		if (light->GetIntensity() != 0 && IsActiveLightVisible())
		{
			for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
			{
				m_Graph->ExecutePass(pass);
			}
		}
	}
}

void PointLightMultiPass::CreatePasses()
{
	AddPass<PointLightShadowPass>();
	AddPass<PointLightShadingPass>();
	AddPass<PointLightWireframePass>();
}

bool PointLightMultiPass::IsActiveLightVisible()
{
	std::shared_ptr<PointLightComponent> light = m_Parameters.Light;

	Transform transform = light->GetWorldTransform();
	transform.SetScale(glm::vec3(light->GetRadius()));

	Camera& camera = m_Parameters.Camera->GetCamera();

	return RenderingHelper::IsLightMeshVisible(m_Parameters.LightMeshVertices, transform, camera);
}
