#include "ResolutionRenderTask.h"
#include "Utils/RenderingHelper.h"
#include "Utils/Files.h"

void ResolutionRenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	m_ResolutionRenderPassSpecification.Name = "Resolution pass";

	m_ResolutionRenderPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1, 1, { FramebufferAttachmentType::Color16, FramebufferAttachmentType::Depth }, TextureType::Texture2D);
	// todo: think if depth is necessary (it is just for icons)

	m_ResolutionRenderPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\resolution-pass.glsl)");
}

void ResolutionRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	m_Renderer->BeginRenderPass(m_ResolutionRenderPassSpecification, glm::mat4(1.0f), glm::mat4(1.0f));

	m_Context->SetShaderDataTexture("u_Light", m_Renderer->GetAntiAliasingOutputTexture());
	m_Context->SetShaderDataFloat("u_Gamma", m_Gamma);

	if (m_Renderer->IsBloomEnabled())
	{
		m_Context->SetShaderDataTexture("u_Bloom", m_Renderer->GetRenderTarget(RenderTarget::Bloom));
		m_Context->SetShaderDataFloat("u_BloomStrength", m_BloomStrength);
		m_Context->SetShaderDataFloat("u_BloomIntensity", m_BloomIntensity);
	}
	else
	{
		m_Context->SetShaderDataTexture("u_Bloom", RenderingHelper::GetWhiteTexture());
		m_Context->SetShaderDataFloat("u_BloomIntensity", 0.0f);
	}

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();
}

void ResolutionRenderTask::Resize(glm::ivec2 size, float upscale)
{
	m_ResolutionRenderPassSpecification.Framebuffer->Resize(size.x * upscale, size.y * upscale, 1);
}

void ResolutionRenderTask::SetGamma(float gamma)
{
	m_Gamma = gamma;
}

float ResolutionRenderTask::GetGamma() const
{
	return m_Gamma;
}

void ResolutionRenderTask::SetBloomStrength(float strength)
{
	m_BloomStrength = strength;
}

float ResolutionRenderTask::GetBloomStrength() const
{
	return m_BloomStrength;
}

void ResolutionRenderTask::SetBloomIntensity(float intensity)
{
	m_BloomIntensity = intensity;
}

float ResolutionRenderTask::GetBloomIntensity() const
{
	return m_BloomIntensity;
}

std::shared_ptr<Texture2D> ResolutionRenderTask::GetTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_ResolutionRenderPassSpecification.Framebuffer->GetAttachment(0));
}

std::shared_ptr<Framebuffer> ResolutionRenderTask::GetFramebuffer() const
{
	return std::static_pointer_cast<Framebuffer>(m_ResolutionRenderPassSpecification.Framebuffer);
}
