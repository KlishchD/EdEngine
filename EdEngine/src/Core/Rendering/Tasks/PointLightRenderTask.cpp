#include "PointLightRenderTask.h"
#include "Utils/RenderingHelper.h"
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

		m_LightPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\light\point-light-pass.glsl)");

		m_LightPassSpecification.SourceFactor = BlendFactor::One;
		m_LightPassSpecification.DestinationFactor = BlendFactor::One;
	}

	{
		m_LightWireframePassSpecification.Name = "Light wire frame pass";

		m_LightWireframePassSpecification.Framebuffer = renderer->GetLightFramebuffer();

		m_LightWireframePassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\wireframe.glsl)");

		m_LightWireframePassSpecification.SourceFactor = BlendFactor::One;
		m_LightWireframePassSpecification.DestinationFactor = BlendFactor::One;
	}

	{
		m_ShadowPassSpecification.Name = "Shadow pass";

		std::shared_ptr<Framebuffer> framebuffer = RenderingHelper::CreateFramebuffer(1, 1, 1, { FramebufferAttachmentType::Depth }, TextureType::CubeTexture, false);

		m_ShadowPassSpecification.Framebuffer = framebuffer;

		m_ShadowPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\light\point-light-shadow-pass.glsl)");

		m_ShadowPassSpecification.bUseBlending = false;

		m_ShadowPassSpecification.bClearDepth = true;
	}
}

void PointLightRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	m_LightPassSpecification.ViewPosition = camera->GetPosition();

	for (std::shared_ptr<Component> component : components)
	{
		if (component->GetType() == ComponentType::PointLight)
		{
			std::shared_ptr<PointLightComponent> light = std::static_pointer_cast<PointLightComponent>(component);

			if (light->GetIntensity() != 0 && m_Renderer->IsLightMeshVisible(light, camera))
			{
				DrawShadowMap(components, light);
				DrawLight(light, camera);
				DrawWireframe(light, camera);
			}
		}
	}
}

void PointLightRenderTask::Resize(glm::ivec2 size, float upscale)
{
	m_ShadowPassSpecification.Framebuffer->Resize(size.x, size.x, size.x);
}

void PointLightRenderTask::DrawShadowMap(const std::vector<std::shared_ptr<Component>>& components, std::shared_ptr<PointLightComponent> light)
{
	if (light->IsShadowCasting())
	{
		m_ShadowPassSpecification.ViewPosition = light->GetPosition();

		m_Renderer->BeginRenderPass(m_ShadowPassSpecification, glm::mat4(1.0f), glm::mat4(1.0f));

		m_Context->SetShaderDataFloat("u_FarPlane", m_Renderer->GetFarPlane());

		for (int32_t i = 0; i < 6; ++i) {
			m_Context->SetShaderDataMat4("u_ViewProjection[" + std::to_string(i) + "]", m_ShadowMapPerspective * light->GetShadowMapPassCameraTransformation(i));
		}

		m_Renderer->SubmitMeshesRaw(components);

		m_Renderer->EndRenderPass();
	}
}

void PointLightRenderTask::DrawLight(std::shared_ptr<PointLightComponent> light, Camera* camera)
{
	m_Renderer->BeginRenderPass(m_LightPassSpecification, camera->GetView(), camera->GetProjection());

	m_Context->SetShaderDataTexture("u_Albedo", m_Renderer->GetRenderTarget(RenderTarget::GAlbedo));
	m_Context->SetShaderDataTexture("u_Position", m_Renderer->GetRenderTarget(RenderTarget::GPosition));
	m_Context->SetShaderDataTexture("u_Normal", m_Renderer->GetRenderTarget(RenderTarget::GNormal));
	m_Context->SetShaderDataTexture("u_RoughnessMetalic", m_Renderer->GetRenderTarget(RenderTarget::GRougnessMetalicEmission));

	m_Context->SetShaderDataFloat("u_FarPlane", m_Renderer->GetFarPlane());

	m_Context->SetShaderDataFloat("u_FilterSize", light->GetShadowFilterSize());
	m_Context->SetShaderDataFloat("u_ShadowMapPixelSize", 1.0f / m_ShadowPassSpecification.Framebuffer->GetWidth()); // TODO : make it vec2

	m_Context->SetShaderDataFloat3("u_PointLight.Position", light->GetPosition());
	m_Context->SetShaderDataFloat3("u_PointLight.Color", light->GetColor());
	m_Context->SetShaderDataFloat("u_PointLight.Intensity", light->GetIntensity());
	m_Context->SetShaderDataFloat("u_PointLight.Radius", light->GetRadius());


	m_Context->SetShaderDataBool("u_PointLight.UseShadowMap", light->IsShadowCasting());

	if (light->IsShadowCasting())
	{
		m_Context->SetShaderDataTexture("u_PointLight.ShadowMap", m_ShadowPassSpecification.Framebuffer->GetDepthAttachment());
	}
	else
	{
		m_Context->SetShaderDataTexture("u_PointLight.ShadowMap", RenderingHelper::GetWhiteTexture());
	}

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

	m_Renderer->SubmitLightMesh(light);

	m_Renderer->EndRenderPass();
}

void PointLightRenderTask::DrawWireframe(std::shared_ptr<PointLightComponent> light, Camera* camera)
{
	if (light->ShouldShowWireframe())
	{
		m_LightWireframePassSpecification.ViewPosition = camera->GetPosition();
		m_Renderer->BeginRenderPass(m_LightWireframePassSpecification, camera->GetView(), camera->GetProjection());

		m_Renderer->SubmitLightMeshWireframe(light);

		m_Renderer->EndRenderPass();
	}
}