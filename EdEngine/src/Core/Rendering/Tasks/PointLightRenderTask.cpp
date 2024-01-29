#include "PointLightRenderTask.h"
#include "Utils/RenderingHelper.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"
#include "Core/Rendering/Framebuffers/CubeFramebuffer.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Utils/Files.h"
#include "SSDORenderTask.h"

void PointLightRenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	m_ShadowMapPerspective = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, m_Renderer->GetFarPlane());

	{
		m_LightPassSpecification.Name = "Light pass";

		m_LightPassSpecification.Framebuffer = renderer->GetLightFramebuffer();

		m_LightPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\point-light-pass.glsl)");

		m_LightPassSpecification.SourceFactor = BlendFactor::One;
		m_LightPassSpecification.DestinationFactor = BlendFactor::One;
	}

	{
		m_ShadowPassSpecification.Name = "Shadow pass";

		std::shared_ptr<CubeFramebuffer> framebuffer = RenderingHelper::CreateCubeFramebuffer(1);
		framebuffer->CreateAttachment(FramebufferAttachmentType::Depth);

		m_ShadowPassSpecification.Framebuffer = framebuffer;

		m_ShadowPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\shadow-pass.glsl)");

		m_ShadowPassSpecification.bUseBlending = false;

		m_ShadowPassSpecification.bClearDepth = true;
	}
}

void PointLightRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	m_LightPassSpecification.ViewPosition = camera->GetPosition();

	for (const std::shared_ptr<Component>& component : components)
	{
		if (std::shared_ptr<PointLightComponent> light = std::dynamic_pointer_cast<PointLightComponent>(component); light && light->GetIntensity() != 0)
		{
			if (light->IsShadowCasting())
			{
				DrawShadowMap(components, light);
			}

			DrawLight(light, camera);
		}
	}
}

void PointLightRenderTask::Resize(glm::ivec2 size, float upscale)
{
	std::shared_ptr<CubeFramebuffer> framebuffer = std::static_pointer_cast<CubeFramebuffer>(m_ShadowPassSpecification.Framebuffer);
	framebuffer->Resize(std::max(size.x, size.y) * upscale);
}

void PointLightRenderTask::DrawShadowMap(const std::vector<std::shared_ptr<Component>>& components, const std::shared_ptr<PointLightComponent>& light)
{
	m_ShadowPassSpecification.ViewPosition = light->GetPosition();

	m_Renderer->BeginRenderPass(m_ShadowPassSpecification, glm::mat4(1.0f), glm::mat4(1.0f));

	m_Context->SetShaderDataFloat("u_FarPlane", m_Renderer->GetFarPlane());

	for (int32_t i = 0; i < 6; ++i) {
		m_Context->SetShaderDataMat4("u_ViewProjection[" + std::to_string(i) + "]", m_ShadowMapPerspective * light->GetShadowMapPassCameraTransformation(i));
	}

	std::static_pointer_cast<CubeFramebuffer>(m_ShadowPassSpecification.Framebuffer)->AttachLayers();

	for (const std::shared_ptr<Component>& component : components)
	{
		if (std::shared_ptr<StaticMeshComponent> meshComponent = std::dynamic_pointer_cast<StaticMeshComponent>(component))
		{
			if (std::shared_ptr<StaticMesh> mesh = meshComponent->GetStaticMesh()) {
				m_Renderer->SubmitMeshRaw(mesh, meshComponent->GetWorldTransform(), meshComponent->GetPreviousWorldTransform());
			}
		}
	}

	m_Renderer->EndRenderPass();
}

void PointLightRenderTask::DrawLight(const std::shared_ptr<PointLightComponent>& light, Camera* camera)
{
	m_Renderer->BeginRenderPass(m_LightPassSpecification, camera->GetView(), camera->GetProjection());

	std::shared_ptr<Shader> shader = m_LightPassSpecification.Shader;

	m_Context->SetShaderDataTexture("u_Albedo", m_Renderer->GetRenderTarget(RenderTarget::GAlbedo));
	m_Context->SetShaderDataTexture("u_Position", m_Renderer->GetRenderTarget(RenderTarget::GPosition));
	m_Context->SetShaderDataTexture("u_Normal", m_Renderer->GetRenderTarget(RenderTarget::GNormal));
	m_Context->SetShaderDataTexture("u_RoughnessMetalic", m_Renderer->GetRenderTarget(RenderTarget::GRougnessMetalicEmission));

	m_Context->SetShaderDataFloat("u_FarPlane", m_Renderer->GetFarPlane());

	m_Context->SetShaderDataFloat("u_FilterSize", m_FilterSize);
	m_Context->SetShaderDataFloat("u_ShadowMapPixelSize", 1.0f / m_ShadowPassSpecification.Framebuffer->GetWidth()); // TODO : make it vec2

	if (std::shared_ptr<SSDORenderTask> ssdo = m_Renderer->GetTask<SSDORenderTask>(); ssdo->IsEnabled() && false) // TODO: take a look at this later ;)
	{
		m_Context->SetShaderDataFloat("u_Radius", ssdo->GetRadius());
		m_Context->SetShaderDataFloat("u_DriectLightStrength", ssdo->GetDirectLightStrength());

		std::vector<glm::vec3> samples = ssdo->GetSamples();

		m_Context->SetShaderDataFloat("u_SampleCount", ssdo->GetSamplesCount());
		for (int32_t i = 0; i < ssdo->GetSamplesCount(); ++i)
		{
			m_Context->SetShaderDataFloat3("u_Samples[" + std::to_string(i) + "]", samples[i]);
		}
	}
	else
	{
		m_Context->SetShaderDataFloat("u_SampleCount", 0.0f);
	}

	m_Context->EnableFaceCulling();
	m_Renderer->SubmitLightMesh(light, m_ShadowPassSpecification.Framebuffer->GetDepthAttachment());
	m_Context->DisableFaceCulling();

	m_Renderer->EndRenderPass();
}
