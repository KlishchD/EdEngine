#include "SpotLightMultiPass.h"
#include "SpotLightShadowPass.h"
#include "SpotLightShadingPass.h"
#include "SpotLightWireframePass.h"
#include "Helpers/GeometryBuilder.h"

void SpotLightMultiPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	MultiPassRenderPass<SpotLightMultiPassParameters>::Initialize(graph);

	m_Parameters.Name = "Spot Light MultiPass";

	{
		auto [vertices, indices] = GeometryBuilder::MakeCone(SpotLightMeshSectorsCount);

		m_Parameters.LightMeshVertices = std::move(vertices);
		m_Parameters.LightMeshIndices = std::move(indices);
	}
}

void SpotLightMultiPass::Execute()
{
	MultiPassRenderPass<SpotLightMultiPassParameters>::Execute();

	for (const std::shared_ptr<SpotLightComponent>& light : m_Parameters.Lights.Get())
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

void SpotLightMultiPass::CreatePasses()
{
	AddPass<SpotLightShadowPass>();
	AddPass<SpotLightShadingPass>();
	AddPass<SpotLightWireframePass>();
}

bool SpotLightMultiPass::IsActiveLightVisible()
{
	std::shared_ptr<SpotLightComponent> light = m_Parameters.Light;

	const float angle = light->GetOuterAngle();
	const float length = light->GetMaxDistance();
	const float radius = glm::tan(angle) * length;

	Transform transform = light->GetWorldTransform();
	transform.SetScale(glm::vec3(radius, length, radius));

	Camera& camera = m_Parameters.Camera->GetCamera();

	return RenderingHelper::IsLightMeshVisible(m_Parameters.LightMeshVertices, transform, camera);
}
