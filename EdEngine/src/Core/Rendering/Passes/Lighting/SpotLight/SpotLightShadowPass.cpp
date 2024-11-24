#include "SpotLightShadowPass.h"

void SpotLightShadowPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<SpotLightShadowPassParameters, SpotLightShadowPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Spot light shadow pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\light\\spot-light-shadow-pass.glsl");

	m_Parameters.bUseBlending = false;
	m_Parameters.bClearDepth = true;
}

void SpotLightShadowPass::Execute()
{
	RenderPass<SpotLightShadowPassParameters, SpotLightShadowPassShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	std::shared_ptr<SpotLightComponent> light = m_Parameters.Light;

	if (light->IsShadowCasting())
	{
		Camera& camera = m_Parameters.Camera->GetCamera();

		glm::mat4 view = glm::lookAt(light->GetPosition(), light->GetPosition() + light->GetWorldTransform().GetRotation() * glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 projection = glm::perspective(light->GetOuterAngle() * 2.0f, camera.GetAspect(), 1.0f, m_Renderer->GetFarPlane());
		
		m_Parameters.ShadowProjectionViewMatrix = projection * view;

		m_Renderer->SetCamera(view, projection, light->GetPosition());

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

    m_Renderer->SpotLightsShadowsDrawn++;
}
