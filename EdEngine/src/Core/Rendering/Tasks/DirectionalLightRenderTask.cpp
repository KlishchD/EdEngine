#include "DirectionalLightRenderTask.h"
#include "Core/Components/DirectionalLightComponent.h"
#include "Utils/RenderingHelper.h"
#include "Utils/MathHelper.h"
#include "Utils/Files.h"

void DirectionalLightRenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	{
		m_LightPassSpecification.Name = "Directional light pass";

		m_LightPassSpecification.Framebuffer = m_Renderer->GetLightFramebuffer();

		m_LightPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\light\directional-light-pass.glsl)");

		m_LightPassSpecification.SourceFactor = BlendFactor::One;
		m_LightPassSpecification.DestinationFactor = BlendFactor::One;
	}

	{
		m_ShadowPassSpecification.Name = "Directional light shadow pass";

		m_ShadowPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1, m_MaxCascadesCount, { FramebufferAttachmentType::Depth }, TextureType::Texture2DArray);

		m_ShadowPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\light\directional-light-shadow-pass.glsl)");

		m_ShadowPassSpecification.bUseBlending = false;
		m_ShadowPassSpecification.bClearDepth = true;
	}
}

void DirectionalLightRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	for (std::shared_ptr<Component> component : components)
	{
		if (component->GetType() == ComponentType::DirectionalLight)
		{
			std::shared_ptr<DirectionalLightComponent> light = std::static_pointer_cast<DirectionalLightComponent>(component);
			CalculateShadowProjections(light, camera);
			DrawShadows(light, components, camera);
			DrawLight(light, camera);
		}
	}
}

void DirectionalLightRenderTask::Resize(glm::ivec2 size, float upscale)
{
	uint32_t sideSize = glm::max((uint32_t)size.x, ShadowCascadeMinSize);
	m_ShadowPassSpecification.Framebuffer->Resize(sideSize, sideSize, m_MaxCascadesCount);
}

std::shared_ptr<Texture2D> DirectionalLightRenderTask::GetShadowTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_ShadowPassSpecification.Framebuffer->GetDepthAttachment());
}

void DirectionalLightRenderTask::CalculateShadowProjections(std::shared_ptr<DirectionalLightComponent> light, Camera* camera)
{
	m_CascadeProjectionViewMatrices.clear();

	if (light->IsShadowCasting())
	{
		static std::vector<glm::vec4> cornersNDC = {
			{ -1.0f, -1.0f, -1.0f, 1.0f },
			{ -1.0f,  1.0f, -1.0f, 1.0f },
			{  1.0f,  1.0f, -1.0f, 1.0f },
			{  1.0f, -1.0f, -1.0f, 1.0f },
			{ -1.0f, -1.0f,  1.0f, 1.0f },
			{ -1.0f,  1.0f,  1.0f, 1.0f },
			{  1.0f,  1.0f,  1.0f, 1.0f },
			{  1.0f, -1.0f,  1.0f, 1.0f }
		};

		int32_t cascades = glm::clamp<int32_t>(light->GetShadowCascadesCount(), MinShadowCascadesCount, MaxShadowCascadesCount);
		float farPlane = m_Renderer->GetFarPlane();

		for (int32_t i = 0; i < cascades; ++i)
		{
			float currentNearPlane = glm::max(1.0f, (1.0f * i / cascades) * farPlane);
			float currentFarPlane = ((i + 1.0f) / cascades) * farPlane;
			glm::mat4 inverseProjectionView = glm::inverse(camera->GetProjectionView(currentNearPlane, currentFarPlane));

			glm::vec3 centerWorld(0.0f);

			std::vector<glm::vec3> cornersWorld;

			for (const glm::vec4& cornerNDC : cornersNDC)
			{
				glm::vec4 cornerWorld = inverseProjectionView * cornerNDC;
				cornerWorld /= cornerWorld.w;
				cornersWorld.emplace_back(cornerWorld);

				centerWorld += glm::vec3(cornerWorld);
			}

			centerWorld /= cornersNDC.size();

			glm::mat4 lightView = glm::lookAt(centerWorld - light->GetDirection(), centerWorld, glm::vec3(0.0f, 1.0f, 0.0f));

			glm::vec3 leftCornerView(std::numeric_limits<float>::max());
			glm::vec3 rightCornerView(std::numeric_limits<float>::min());

			for (const glm::vec3& cornerWorld : cornersWorld)
			{
				glm::vec3 cornerView = lightView * glm::vec4(cornerWorld, 1.0f);

				leftCornerView = MathHelper::MinPerComponent(leftCornerView, cornerView);
				rightCornerView = MathHelper::MaxPerComponent(rightCornerView, cornerView);
			}

			float size = glm::max(glm::distance(cornersWorld[0], cornersWorld[6]), glm::distance(cornersWorld[0], cornersWorld[2]));

			float width = rightCornerView.x - leftCornerView.x;
			float height = rightCornerView.y - leftCornerView.y;

			size = glm::max(size, glm::min(width, height));

			{
				float diff = size - width;
				leftCornerView.x -= diff / 2.0f;
				rightCornerView.x += diff / 2.0f;
			}

			{
				float diff = size - height;
				leftCornerView.y -= diff / 2.0f;
				rightCornerView.y += diff / 2.0f;
			}

			leftCornerView.z *= light->GetShadowMapZMultiplier();
			rightCornerView.z *= light->GetShadowMapZMultiplier();

			float pixelSize = size / m_ShadowPassSpecification.Framebuffer->GetWidth();

			leftCornerView.x = std::round(leftCornerView.x / pixelSize) * pixelSize;
			leftCornerView.y = std::round(leftCornerView.y / pixelSize) * pixelSize;
			rightCornerView.x = std::round(rightCornerView.x / pixelSize) * pixelSize;
			rightCornerView.y = std::round(rightCornerView.y / pixelSize) * pixelSize;

			glm::mat4 lightProjection = glm::ortho(leftCornerView.x, rightCornerView.x, leftCornerView.y, rightCornerView.y, leftCornerView.z, rightCornerView.z);

			m_CascadeProjectionViewMatrices.push_back(lightProjection * lightView);
		}
	}
}

void DirectionalLightRenderTask::DrawShadows(std::shared_ptr<DirectionalLightComponent> light, const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	if (light->IsShadowCasting())
	{
		m_Renderer->BeginRenderPass(m_ShadowPassSpecification);

		m_Context->SetShaderDataFloat("u_Cascades", m_CascadeProjectionViewMatrices.size());
		for (int32_t i = 0; i < m_CascadeProjectionViewMatrices.size(); ++i)
		{
			m_Context->SetShaderDataMat4("u_ProjectionViewMatries[" + std::to_string(i) + "]", m_CascadeProjectionViewMatrices[i]);
		}

		m_Renderer->SubmitMeshesRaw(components);

		m_Renderer->EndRenderPass();
	}
}

void DirectionalLightRenderTask::DrawLight(std::shared_ptr<DirectionalLightComponent> light, Camera* camera)
{
	m_LightPassSpecification.ViewPosition = camera->GetPosition();
	m_Renderer->BeginRenderPass(m_LightPassSpecification, camera->GetView(), camera->GetProjection());

	m_Context->SetShaderDataTexture("u_Albedo", m_Renderer->GetRenderTarget(RenderTarget::GAlbedo));
	m_Context->SetShaderDataTexture("u_Position", m_Renderer->GetRenderTarget(RenderTarget::GPosition));
	m_Context->SetShaderDataTexture("u_Normal", m_Renderer->GetRenderTarget(RenderTarget::GNormal));
	m_Context->SetShaderDataTexture("u_RoughnessMetalic", m_Renderer->GetRenderTarget(RenderTarget::GRougnessMetalicEmission));

	m_Context->SetShaderDataFloat("u_FarPlane", m_Renderer->GetFarPlane());

	m_Context->SetShaderDataFloat3("u_Light.Color", light->GetColor());
	m_Context->SetShaderDataFloat("u_Light.Intensity", light->GetIntensity());
	m_Context->SetShaderDataFloat3("u_Light.Direction", light->GetDirection());

	if (light->IsShadowCasting())
	{
		m_Context->SetShaderDataBool("u_Light.IsShadowCasting", true);
		m_Context->SetShaderDataTexture("u_Light.ShadowMap", m_ShadowPassSpecification.Framebuffer->GetDepthAttachment());
		
		m_Context->SetShaderDataFloat("u_Light.ShadowMapPixelSize", 1.0f / m_ShadowPassSpecification.Framebuffer->GetWidth());

		m_Context->SetShaderDataFloat("u_Light.ShadowFilterSize", light->GetShadowFilterSize());
		m_Context->SetShaderDataFloat("u_Light.ShadowFilterRadius", light->GetShadowFilterRadius());

		m_Context->SetShaderDataFloat("u_Light.Cascades", m_CascadeProjectionViewMatrices.size());
		for (int32_t i = 0; i < m_CascadeProjectionViewMatrices.size(); ++i)
		{
			m_Context->SetShaderDataMat4("u_Light.ShadowProjectionViewMatries[" + std::to_string(i) + "]", m_CascadeProjectionViewMatrices[i]);
		}
	}
	else
	{
		m_Context->SetShaderDataBool("u_Light.IsShadowCasting", false);
	}

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();
}