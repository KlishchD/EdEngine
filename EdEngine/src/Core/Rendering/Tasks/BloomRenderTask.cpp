#include "BloomRenderTask.h"
#include "Utils/Files.h"
#include "Utils/RenderingHelper.h"

void BloomRenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	m_BloomFramebuffer = RenderingHelper::CreateFramebuffer(1, 1, 1, { FramebufferAttachmentType::Bloom }, TextureType::Texture2D);

	m_BloomIntermediateTextrues.push_back(std::static_pointer_cast<Texture2D>(m_BloomFramebuffer->GetAttachment(0)));
	for (int32_t i = 0; i < m_BloomDownscaleCount; ++i)
	{
		m_BloomIntermediateTextrues.push_back(RenderingHelper::CreateBloomIntermediateTexture());
	}

	m_BloomDownscaleShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\bloom\downscale.glsl)");
	m_BloomUpscaleShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\bloom\upscale.glsl)");
}

void BloomRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	if (m_bIsEnabled)
	{
		std::shared_ptr<Texture2D> scene = m_Renderer->GetAntiAliasingOutputTexture();

		BloomDownscale(scene, m_BloomIntermediateTextrues[1]);
		for (int32_t i = 2; i < m_BloomDownscaleCount; ++i)
		{
			BloomDownscale(m_BloomIntermediateTextrues[i - 1], m_BloomIntermediateTextrues[i]);
		}

		for (int32_t i = m_BloomDownscaleCount - 1; i > 1; --i)
		{
			BloomUpscale(m_BloomIntermediateTextrues[i], m_BloomIntermediateTextrues[i - 1]);
		}

		BloomUpscale(m_BloomIntermediateTextrues[1], m_BloomIntermediateTextrues[0], scene);
	}
}

void BloomRenderTask::Resize(glm::ivec2 size, float upscale)
{
	m_Size = size;

	for (int32_t i = 0; i < m_BloomDownscaleCount; ++i)
	{
		size /= 2.0f;
		m_BloomIntermediateTextrues[i]->Resize(std::max(size.x, 1), std::max(size.y, 1));
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
	m_BloomDownscaleCount = count;
	while (m_BloomIntermediateTextrues.size() < count)
	{
		m_BloomIntermediateTextrues.push_back(RenderingHelper::CreateBloomIntermediateTexture());
	}

	Resize(m_Size, 1.0f);
}

uint32_t BloomRenderTask::GetBloomDownscaleTextureCount() const
{
	return m_BloomDownscaleCount;
}

std::shared_ptr<Texture2D> BloomRenderTask::GetTexture()
{
	return m_BloomIntermediateTextrues[0];
}

void BloomRenderTask::BloomDownscale(std::shared_ptr<Texture2D> in, std::shared_ptr<Texture2D> out)
{
	m_BloomFramebuffer->SetAttachment(0, out, FramebufferSizeAdjustmentMode::ResizeFramebufferToTexutreSize);
	
	m_Renderer->BeginRenderPass("Bloom downscale", m_BloomFramebuffer, m_BloomDownscaleShader, glm::mat4(1.0f), glm::mat4(1.0f), glm::vec3(0.0f));
	
	m_Context->SetShaderDataTexture("u_Input", in);
	m_Context->SetShaderDataFloat2("u_InPixelSize", glm::vec2(1.0f / in->GetWidth(), 1.0f / in->GetHeight()));
	m_Context->SetShaderDataFloat2("u_OutPixelSize", glm::vec2(1.0f / out->GetWidth(), 1.0f / out->GetHeight()));
	
	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);
	
	m_Renderer->EndRenderPass();
}

void BloomRenderTask::BloomUpscale(std::shared_ptr<Texture2D> downscaled, std::shared_ptr<Texture2D> upscaled, std::shared_ptr<Texture2D> fullsize)
{
	m_BloomFramebuffer->SetAttachment(0, upscaled, FramebufferSizeAdjustmentMode::ResizeFramebufferToTexutreSize);
	
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
