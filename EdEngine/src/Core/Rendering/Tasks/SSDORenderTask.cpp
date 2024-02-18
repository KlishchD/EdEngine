#include "SSDORenderTask.h"
#include "Utils/RenderingHelper.h"
#include "Utils/MathHelper.h"
#include "Utils/Files.h"

void SSDORenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	{
		m_SSDOPassSpecification.Name = "SSDO pass";

		m_SSDOPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1, 1, { FramebufferAttachmentType::Color16 }, TextureType::Texture2D);

		m_SSDOPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\SSDO.glsl)");

		m_SSDOPassSpecification.bUseBlending = false;
		m_SSDOPassSpecification.bUseDepthTesting = false;
	}

	{
		m_SSDOBlurPassSpecification.Name = "SSDO blur pass";

		m_SSDOBlurPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1, 1, { FramebufferAttachmentType::Color16 }, TextureType::Texture2D);

		m_SSDOBlurPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\SSDO-blur.glsl)");

		m_SSDOBlurPassSpecification.bUseBlending = false;
		m_SSDOBlurPassSpecification.bUseDepthTesting = false;
	}


	m_IndirectLightShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\indirect-light-pass.glsl)");

	SetSamplesCount(m_SamplesCount);
}

void SSDORenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	if (m_bIsEnabled)
	{
		SSDO(camera);
		Blur();

		m_Renderer->BeginRenderPass("Indirect light", m_Renderer->GetLightFramebuffer(), m_IndirectLightShader);
		
		m_Context->EnableBlending(BlendFactor::One, BlendFactor::One);

		m_Context->SetShaderDataTexture("u_SSDO", m_SSDOBlurPassSpecification.Framebuffer->GetAttachment(0));

		m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

		m_Renderer->EndRenderPass();
	}
}

void SSDORenderTask::Resize(glm::ivec2 size, float upscale)
{
	m_SSDOPassSpecification.Framebuffer->Resize(size.x, size.y, 1);
	m_SSDOBlurPassSpecification.Framebuffer->Resize(size.x, size.y, 1);
}

void SSDORenderTask::SetSamplesCount(int32_t count)
{
	m_SamplesCount = count;
	m_Samples = MathHelper::GenerateHalfSphereSamples(count, false);
}

int32_t SSDORenderTask::GetSamplesCount() const
{
	return m_SamplesCount;
}

void SSDORenderTask::SetIndirectStrength(float strength)
{
	m_IndirectStrength = strength;
}

float SSDORenderTask::GetIndirectStrength() const
{
	return m_IndirectStrength;
}

void SSDORenderTask::SetDirectLightStrength(float strength)
{
	m_DriectLightStrength = strength;
}

float SSDORenderTask::GetDirectLightStrength() const
{
	return m_DriectLightStrength;
}

void SSDORenderTask::SetRadius(float radius)
{
	m_Radius = radius;
}

float SSDORenderTask::GetRadius() const
{
	return m_Radius;
}

void SSDORenderTask::SetBlurFilterSize(int32_t size)
{
	m_BlurFilterSize = size;
}

int32_t SSDORenderTask::GetBlurFilterSize() const
{
	return m_BlurFilterSize;
}

std::vector<glm::vec3> SSDORenderTask::GetSamples() const
{
	return m_Samples;
}

std::shared_ptr<Texture2D> SSDORenderTask::GetTexutre() const
{
	return std::static_pointer_cast<Texture2D>(m_SSDOBlurPassSpecification.Framebuffer->GetAttachment(0));
}

void SSDORenderTask::SSDO(Camera* camera)
{
	glm::mat4 view = camera->GetView();
	m_Renderer->BeginRenderPass(m_SSDOPassSpecification, view, camera->GetProjection());

	m_Context->SetShaderDataMat4("u_NormalMatrix", glm::transpose(glm::inverse(view)));

	m_Context->SetShaderDataTexture("u_Diffuse", m_Renderer->GetRenderTarget(RenderTarget::Diffuse));
	m_Context->SetShaderDataTexture("u_Albedo", m_Renderer->GetRenderTarget(RenderTarget::GAlbedo));
	m_Context->SetShaderDataTexture("u_RoughnessMetalic", m_Renderer->GetRenderTarget(RenderTarget::GRougnessMetalicEmission));
	m_Context->SetShaderDataTexture("u_Position", m_Renderer->GetRenderTarget(RenderTarget::GPosition));
	m_Context->SetShaderDataTexture("u_Normal", m_Renderer->GetRenderTarget(RenderTarget::GNormal));

	m_Context->SetShaderDataFloat("u_IndirectStrength", m_IndirectStrength);
	m_Context->SetShaderDataFloat("u_Radius", m_Radius);

	m_Context->SetShaderDataFloat("u_SampleCount", m_SamplesCount);
	for (int32_t i = 0; i < m_SamplesCount; ++i)
	{
		m_Context->SetShaderDataFloat3("u_Samples[" + std::to_string(i) + "]", m_Samples[i]);
	}

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();

}

void SSDORenderTask::Blur()
{
	m_Renderer->BeginRenderPass(m_SSDOBlurPassSpecification);

	m_Context->SetShaderDataTexture("u_Input", m_SSDOPassSpecification.Framebuffer->GetAttachment(0));
	m_Context->SetShaderDataFloat("u_FilerSize", m_BlurFilterSize);

	glm::vec2 size = glm::vec2(1.0f / m_SSDOPassSpecification.Framebuffer->GetWidth(), 1.0f / m_SSDOPassSpecification.Framebuffer->GetHeight());
	m_Context->SetShaderDataFloat2("u_PixelSize", size);

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();
}
