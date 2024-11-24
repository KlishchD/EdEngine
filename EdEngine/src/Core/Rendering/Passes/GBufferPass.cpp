#include "GBufferPass.h"
#include "Helpers/MathHelper.h"
#include "Core/Components/StaticMeshComponent.h"

void GBufferPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<GBufferPassParameters, GBufferPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "GBuffer pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\deferred\\geometry-pass.glsl");

	m_Parameters.bUseBlending = false;

	m_Parameters.bClearColors = true;
	m_Parameters.bClearDepth = true;

	for (int32_t i = 0; i < m_JitterSequenceSize; ++i)
	{
		m_JitterSequence.push_back(glm::vec2(2.0f * MathHelper::Halton(i + 1, 2) - 1.0f, 2.0f * MathHelper::Halton(i + 1, 3) - 1.0f));
	}
}

void GBufferPass::Execute()
{
	RenderPass<GBufferPassParameters, GBufferPassShaderParameters>::Execute();

	glm::vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	SetCameraInformation();

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
					m_ShaderParameters.Material_BaseColor = material->GetBaseColor();

					SetTextureOrWhite(m_ShaderParameters.Material_BaseColorTexture, material->GetBaseColorTexture());
					SetTextureOrWhite(m_ShaderParameters.Material_NormalTexture, material->GetNormalTexture());
					SetTextureOrWhite(m_ShaderParameters.Material_RoughnessTexture, material->GetRoughnessTexture());
					SetTextureOrWhite(m_ShaderParameters.Material_MetalicTexture, material->GetMetalicTexture());
					
					m_ShaderParameters.Material_PerformNormalMapping = material->ShouldPerformNormalMapping();

					m_ShaderParameters.PreviousModelMatrix = previousWorldTransform.GetMatrix();
					m_ShaderParameters.ModelMatrix = worldTransform.GetMatrix();
					m_ShaderParameters.NormalMatrix = worldTransform.GetInversedTransposedMatrix();

					m_ShaderParameters.Material_Roughness = material->GetRoughness();
					m_ShaderParameters.Material_Metalic = material->GetMetalic();
					m_ShaderParameters.Material_Emission = material->GetEmission();

					SubmitShaderParameters();

					m_Context->SetVertexBuffer(submesh->GetVertexBuffer());
					m_Context->SetIndexBuffer(submesh->GetIndexBuffer());
					m_Context->Draw();

					m_Renderer->StaticMeshesDrawn++;
				}
			}
		}
	}
}

void GBufferPass::SetCameraInformation()
{
	Camera& camera = m_Parameters.Camera->GetCamera();

	glm::mat4 view = camera.GetView();
	glm::mat4 projection = camera.GetProjection();

	glm::vec2 size = glm::vec2(m_Parameters.DrawFramebuffer->GetWidth(), m_Parameters.DrawFramebuffer->GetHeight());

	glm::vec2 jitter = m_JitterSequence[m_CurrentJitterIndex] / size;
	glm::vec2 previousJitter = m_JitterSequence[(m_CurrentJitterIndex - 1 + m_JitterSequenceSize) % m_JitterSequenceSize] / size;

	bool bIsTAAEnabled = m_Renderer->GetAAMethod() == AAMethod::TAA;

	if (bIsTAAEnabled)
	{
		projection = glm::translate(glm::mat4(1.0f), glm::vec3(jitter - previousJitter, 0.0f)) * projection;
		camera.SetProjection(projection);
	}

	m_Renderer->SetCamera(view, projection, camera.GetPosition());

	if (bIsTAAEnabled)
	{
		m_Context->SetShaderDataFloat2("u_PreviousJitter", previousJitter);
		m_Context->SetShaderDataFloat2("u_Jitter", jitter);
	}
	else
	{
		m_Context->SetShaderDataMat4("u_PreviousProjectionMatrix", projection);
	}

	m_Context->SetShaderDataMat4("u_PreviousViewMatrix", m_PreviousView);

	m_CurrentJitterIndex = (m_CurrentJitterIndex + 1) % m_JitterSequenceSize;
	m_PreviousView = view;
}
