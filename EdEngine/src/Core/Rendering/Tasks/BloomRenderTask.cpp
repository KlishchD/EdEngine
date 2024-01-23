#include "BloomRenderTask.h"
#include "Utils/Files.h"
#include "Utils/RenderingHelper.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"

void BloomRenderTask::Setup(Renderer* renderer)
{
	m_BloomFramebuffer = RenderingHelper::CreateFramebuffer(1, 1);
	m_BloomFramebuffer->CreateAttachment(FramebufferAttachmentType::Bloom);

	m_BloomIntermediateTextrues.push_back(std::static_pointer_cast<Texture2D>(m_BloomFramebuffer->GetAttachment(0)));
	for (int32_t i = 0; i < m_NewBloomDownscaleCount; ++i)
	{
		m_BloomIntermediateTextrues.push_back(RenderingHelper::CreateBloomIntermediateTexture());
	}

	m_BloomDownscaleShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\bloom\downscale.glsl)");
	m_BloomUpscaleShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\bloom\upscale.glsl)");
}

void BloomRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	std::shared_ptr<Texture2D> scene = m_Renderer->GetAntiAliasingOutputTexture();

	BloomDownscale(scene, m_BloomIntermediateTextrues[1]);
	for (int32_t i = 2; i < m_NewBloomDownscaleCount; ++i)
	{
		BloomDownscale(m_BloomIntermediateTextrues[i - 1], m_BloomIntermediateTextrues[i]);
	}

	for (int32_t i = m_NewBloomDownscaleCount - 1; i > 1; --i)
	{
		BloomUpscale(m_BloomIntermediateTextrues[i], m_BloomIntermediateTextrues[i - 1]);
	}

	BloomUpscale(m_BloomIntermediateTextrues[1], m_BloomIntermediateTextrues[0], scene);
}

void BloomRenderTask::Resize(glm::ivec2 size, float upscale)
{
	m_Size = size;

	for (int32_t i = 0; i < m_NewBloomDownscaleCount; ++i)
	{
		size /= 2.0f;
		m_BloomIntermediateTextrues[i]->Resize(size.x, size.y);
	}
}

void BloomRenderTask::SetBloomMixStrength(float strength)
{
	m_BloomMixStrength = strength;
}

float BloomRenderTask::GetBloomMixStrength() const
{
	return m_BloomMixStrength;
}

void BloomRenderTask::SetBloomDownscaleTexturesCount(uint32_t count)
{
	m_NewBloomDownscaleCount = count;
	while (m_BloomIntermediateTextrues.size() < count)
	{
		m_BloomIntermediateTextrues.push_back(RenderingHelper::CreateBloomIntermediateTexture());
	}

	Resize(m_Size, 1.0f);
}

void BloomRenderTask::BloomDownscale(const std::shared_ptr<Texture2D>& in, const std::shared_ptr<Texture2D>& out)
{
	m_BloomFramebuffer->SetAttachment(0, out, true);
	
	m_Renderer->BeginRenderPass("Bloom downscale", m_BloomFramebuffer, m_BloomDownscaleShader, glm::mat4(1.0f), glm::mat4(1.0f), glm::vec3(0.0f));
	
	m_Context->SetShaderDataTexture("u_Input", in);
	m_Context->SetShaderDataFloat2("u_InPixelSize", glm::vec2(1.0f / in->GetWidth(), 1.0f / in->GetHeight()));
	m_Context->SetShaderDataFloat2("u_OutPixelSize", glm::vec2(1.0f / out->GetWidth(), 1.0f / out->GetHeight()));
	
	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);
	
	m_Renderer->EndRenderPass();
}

void BloomRenderTask::BloomUpscale(const std::shared_ptr<Texture2D>& downscaled, const std::shared_ptr<Texture2D>& upscaled, const std::shared_ptr<Texture2D>& fullsize)
{
	m_BloomFramebuffer->SetAttachment(0, upscaled, true);
	
	m_Renderer->BeginRenderPass("Bloom downscale", m_BloomFramebuffer, m_BloomUpscaleShader, glm::mat4(1.0f), glm::mat4(1.0f), glm::vec3(0.0f));
	
	m_Context->SetShaderDataTexture("u_Downscaled", downscaled);
	m_Context->SetShaderDataTexture("u_Upscaled", upscaled);
	m_Context->SetShaderDataFloat2("u_UpscaledPixelSize", glm::vec2(1.0f / upscaled->GetWidth(), 1.0f / upscaled->GetHeight()));
	m_Context->SetShaderDataFloat2("u_DownscaledPixelSize", glm::vec2(1.0f / downscaled->GetWidth(), 1.0f / downscaled->GetHeight()));
	m_Context->SetShaderDataFloat("u_MixStrength", m_BloomMixStrength);

	if (fullsize)
	{
		m_Context->SetShaderDataBool("u_UseFullsize", true);
		m_Context->SetShaderDataTexture("u_Fullsize", fullsize);
	}
	else
	{
		m_Context->SetShaderDataBool("u_UseFullsize", false);
	}
	
	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);
	
	m_Renderer->EndRenderPass();
}
