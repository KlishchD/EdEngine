#include "PointLightShadowPass.h"
#include "Core/Assets/StaticMesh.h"

void PointLightShadowPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<PointLightShadowPassParameters, PointLightShadowPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Point light shadow pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\light\\point-light-shadow-pass.glsl");

	m_Parameters.bUseBlending = false;
	m_Parameters.bClearDepth = true;
}

void PointLightShadowPass::Execute()
{
	RenderPass<PointLightShadowPassParameters, PointLightShadowPassShaderParameters>::Execute();

	std::shared_ptr<PointLightComponent> light = m_Parameters.Light;

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.x, size.x);

	if (light->IsShadowCasting())
	{
		static const glm::mat4 perspective = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, m_Renderer->GetFarPlane());

		for (int32_t i = 0; i < 6; ++i)
		{
			m_ShaderParameters.ViewProjection[i] = perspective * light->GetShadowMapPassCameraTransformation(i);
		}

		m_ShaderParameters.FarPlane = m_Renderer->GetFarPlane();

		m_ShaderParameters.ViewPosition = light->GetPosition();

		for (const std::shared_ptr<StaticMeshComponent>& component : m_Parameters.Meshes.Get())
		{
			if (std::shared_ptr<StaticMesh> mesh = component->GetStaticMesh())
			{
				Transform worldTransform = component->GetWorldTransform();
				Transform previousWorldTransform = component->GetPreviousWorldTransform();

				for (const std::shared_ptr<StaticSubmesh>& submesh : mesh->GetSubmeshes())
				{
					if (std::shared_ptr<Material> material = submesh->GetMaterial())
					{
						m_ShaderParameters.ModelMatrix = worldTransform.GetMatrix();

						SubmitShaderParameters();

						m_Context->SetVertexBuffer(submesh->GetVertexBuffer());
						m_Context->SetIndexBuffer(submesh->GetIndexBuffer());
						m_Context->Draw();
					}
				}
			}
		}
	}

    m_Renderer->PointLightsShadowsDrawn++;
}