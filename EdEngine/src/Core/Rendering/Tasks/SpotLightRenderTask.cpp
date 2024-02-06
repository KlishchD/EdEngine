#include "SpotLightRenderTask.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/SpotLightComponent.h"
#include "Utils/RenderingHelper.h"
#include "Utils/MathHelper.h"
#include "Utils/Files.h"
#include <random>

void SpotLightRenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	{
		m_ShadowPassSpecification.Name = "Spot light shadow pass";

		m_ShadowPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
		m_ShadowPassSpecification.Framebuffer->CreateAttachment(FramebufferAttachmentType::Depth);

		m_ShadowPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\light\spot-light-shadow-pass.glsl)");

		m_ShadowPassSpecification.bUseBlending = false;
		m_ShadowPassSpecification.bClearDepth = true;

	}

	{
		Texture2DData data;
		data.Width = 1;
		data.Height = 1;
		data.Data = nullptr;

		Texture2DImportParameters parameters;
		parameters.Format = PixelFormat::RG32F;
		parameters.Filtering = FilteringMode::Nearest;

		m_ShadowSamples = RenderingHelper::CreateTexture2D(std::move(parameters), std::move(data), "Spot light shadow samples");

		UpdateShadowSamplesTexture();
	}

	{
		m_LightPassSpecification.Name = "Spot light pass";

		m_LightPassSpecification.Framebuffer = m_Renderer->GetLightFramebuffer();

		m_LightPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\light\spot-light-pass.glsl)");

		m_LightPassSpecification.SourceFactor = BlendFactor::One;
		m_LightPassSpecification.DestinationFactor = BlendFactor::One;
	}

	{
		m_LightMeshWireframePassSpecification.Name = "Spot wire frame light pass";

		m_LightMeshWireframePassSpecification.Framebuffer = m_Renderer->GetLightFramebuffer();

		m_LightMeshWireframePassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\wireframe.glsl)");

		m_LightMeshWireframePassSpecification.SourceFactor = BlendFactor::One;
		m_LightMeshWireframePassSpecification.DestinationFactor = BlendFactor::OneMinusSourceAlpha;
	}
}

void SpotLightRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	for (const std::shared_ptr<Component> component : components)
	{
		if (component->GetType() == ComponentType::SpotLight)
		{
			std::shared_ptr<SpotLightComponent> light = std::static_pointer_cast<SpotLightComponent>(component);
			DrawShadows(components, light);
			DrawLight(camera, light);
			DrawWireframe(camera, light);
		}
	}
}

void SpotLightRenderTask::Resize(glm::ivec2 size, float upscale)
{
	std::shared_ptr<Framebuffer> framebuffer = std::static_pointer_cast<Framebuffer>(m_ShadowPassSpecification.Framebuffer);
	framebuffer->Resize(size.x, size.y);
	m_Aspect = 1.0f * size.x / size.y;
	m_ShadowMapPixelSize = glm::vec2(1.0f / size.x, 1.0f / size.y);
}

void SpotLightRenderTask::SetShadowSamplesBlockCount(int32_t count)
{
	m_ShadowSamplesBlockCount = count;
	UpdateShadowSamplesTexture();
}

int32_t SpotLightRenderTask::GetShadowSamplesBlocksCount() const
{
	return m_ShadowSamplesBlockCount;
}

void SpotLightRenderTask::SetShadowFilterSize(int32_t size)
{
	m_ShadowFilterSize = size;
	UpdateShadowSamplesTexture();
}

int32_t SpotLightRenderTask::GetShadowFilterSize() const
{
	return m_ShadowFilterSize;
}

void SpotLightRenderTask::SetShadowFilterRadius(float radius)
{
	m_ShadowFilterRadius = radius;
}

float SpotLightRenderTask::GetShadowFilterRadius() const
{
	return m_ShadowFilterRadius;
}

void SpotLightRenderTask::DrawShadows(const std::vector<std::shared_ptr<Component>>& components, const std::shared_ptr<SpotLightComponent>& light)
{
	if (light->IsShadowCasting())
	{
		m_ShadowPassSpecification.ViewPosition = light->GetPosition();
		glm::mat4 view = glm::lookAt(light->GetPosition(), light->GetPosition() + light->GetWorldTransform().GetRotation() * glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 projection = glm::perspective(light->GetOuterAngle() * 2.0f, m_Aspect, 1.0f, m_Renderer->GetFarPlane());
		m_ShadowProjectionViewMatrix = projection * view;
		m_Renderer->BeginRenderPass(m_ShadowPassSpecification, view, projection);

		m_Renderer->SubmitMeshesRaw(components);

		m_Renderer->EndRenderPass();
	}
}

void SpotLightRenderTask::DrawLight(Camera* camera, const std::shared_ptr<SpotLightComponent>& light)
{
	{
		m_LightPassSpecification.ViewPosition = camera->GetPosition();
		m_Renderer->BeginRenderPass(m_LightPassSpecification, camera->GetView(), camera->GetProjection());

		m_Context->SetShaderDataTexture("u_Albedo", m_Renderer->GetRenderTarget(RenderTarget::GAlbedo));
		m_Context->SetShaderDataTexture("u_Position", m_Renderer->GetRenderTarget(RenderTarget::GPosition));
		m_Context->SetShaderDataTexture("u_Normal", m_Renderer->GetRenderTarget(RenderTarget::GNormal));
		m_Context->SetShaderDataTexture("u_RoughnessMetalic", m_Renderer->GetRenderTarget(RenderTarget::GRougnessMetalicEmission));

		m_Context->SetShaderDataFloat3("u_SpotLight.Color", light->GetColor());
		m_Context->SetShaderDataFloat("u_SpotLight.MaxDistance", light->GetMaxDistance());
		m_Context->SetShaderDataFloat("u_SpotLight.Intensity", light->GetIntensity());
		m_Context->SetShaderDataFloat3("u_SpotLight.Position", light->GetPosition());
		m_Context->SetShaderDataFloat("u_SpotLight.InnerAngleCos", glm::cos(light->GetInnerAngle()));
		m_Context->SetShaderDataFloat("u_SpotLight.OuterAngleCos", glm::cos(light->GetOuterAngle()));

		m_Context->SetShaderDataBool("u_SpotLight.IsShadowCasting", light->IsShadowCasting());
		m_Context->SetShaderDataFloat("u_FarPlane", m_Renderer->GetFarPlane());

		if (light->IsShadowCasting())
		{
			m_Context->SetShaderDataTexture("u_SpotLight.ShadowMap", m_ShadowPassSpecification.Framebuffer->GetDepthAttachment());
			m_Context->SetShaderDataFloat2("u_SpotLight.ShadowMapPixelSize", m_ShadowMapPixelSize);
			
			m_Context->SetShaderDataMat4("u_SpotLight.ShadowProjectionViewMatrix", m_ShadowProjectionViewMatrix);

			m_Context->SetShaderDataTexture("u_SpotLight.ShadowSamples", m_ShadowSamples);
			m_Context->SetShaderDataFloat2("u_SpotLight.ShadowSamplesPixelSize", glm::vec2(1.0f / (m_ShadowFilterSize * m_ShadowSamplesBlockCount), 1.0f / m_ShadowFilterSize));
			m_Context->SetShaderDataFloat("u_SpotLight.ShadowFilterSize", m_ShadowFilterSize);
			m_Context->SetShaderDataFloat("u_SpotLight.ShadowFilterRadius", m_ShadowFilterRadius);
		}
		else
		{
			m_Context->SetShaderDataTexture("u_SpotLight.ShadowMap", RenderingHelper::GetWhiteTexture());
		}

		m_Renderer->SubmitLightMesh(light);

		m_Renderer->EndRenderPass();
	}
}

void SpotLightRenderTask::DrawWireframe(Camera* camera, const std::shared_ptr<SpotLightComponent>& light)
{
	if (light->ShouldShowWireframe())
	{
		m_LightMeshWireframePassSpecification.ViewPosition = camera->GetPosition();
		m_Renderer->BeginRenderPass(m_LightMeshWireframePassSpecification, camera->GetView(), camera->GetProjection());

		m_Renderer->SubmitLightMeshWireframe(light);

		m_Renderer->EndRenderPass();
	}
}

void SpotLightRenderTask::UpdateShadowSamplesTexture()
{
	std::vector<glm::vec2> samples = MathHelper::GenerateCircleSamples(m_ShadowFilterSize * m_ShadowFilterSize * m_ShadowSamplesBlockCount);

	Texture2DData data;

	data.Width = m_ShadowFilterSize * m_ShadowSamplesBlockCount;
	data.Height = m_ShadowFilterSize;
	data.Data = (uint8_t*) samples.data();
	data.PixelSize = Types::GetPixelSize(m_ShadowSamples->GetPixelFormat());

	m_ShadowSamples->SetData(data);

	data.Data = nullptr;
}
