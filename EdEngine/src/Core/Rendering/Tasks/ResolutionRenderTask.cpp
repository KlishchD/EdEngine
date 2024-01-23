#include "ResolutionRenderTask.h"
#include "Utils/RenderingHelper.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"
#include "Utils/Files.h"

void ResolutionRenderTask::Setup(Renderer* renderer)
{
	m_ResolutionRenderPassSpecification.Name = "Resolution pass";

	m_ResolutionRenderPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
	m_ResolutionRenderPassSpecification.Framebuffer->CreateAttachment(FramebufferAttachmentType::Color16);

	m_ResolutionRenderPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\resolution-pass.glsl)");
}

void ResolutionRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	m_Renderer->BeginRenderPass(m_ResolutionRenderPassSpecification, glm::mat4(1.0f), glm::mat4(1.0f));

	m_Context->SetShaderDataTexture("u_Color", m_Renderer->GetAntiAliasingOutputTexture());
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
	{
		std::shared_ptr<Framebuffer> framebuffer = std::static_pointer_cast<Framebuffer>(m_ResolutionRenderPassSpecification.Framebuffer);
		framebuffer->Resize(size.x * upscale, size.y * upscale);
	}
}

void ResolutionRenderTask::SetGamma(float gamma)
{
	m_Gamma = gamma;
}

float ResolutionRenderTask::GetGamma() const
{
	return m_Gamma;
}
