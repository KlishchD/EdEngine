#include "TAARenderTask.h"
#include "Utils/Files.h"
#include "Utils/RenderingHelper.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"

void TAARenderTask::Setup(Renderer* renderer)
{
	m_TAAShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\AA\TAA.glsl)");

	m_AAFramebuffer = m_Renderer->GetAntiAliasingFrameBuffer();
	m_HistoryBuffer.push_back(m_AAFramebuffer->GetAttachment(0));

	while (m_HistoryBuffer.size() < m_HistoryBufferSize)
	{
		Texture2DImportParameters parameters;
		parameters.WrapS = WrapMode::ClampToEdge;
		parameters.WrapT = WrapMode::ClampToEdge;
		parameters.Format = PixelFormat::RGBA16F;

		Texture2DData data;
		data.Width = 1;
		data.Height = 1;
		data.Data = nullptr;

		m_HistoryBuffer.push_back(RenderingHelper::CreateTexture2D(parameters, data, "History buffer texture"));
	}
}

void TAARenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	if (m_HistoryBufferSize > 1)
	{
		m_ActiveHistoryBufferTextureIndex = (m_ActiveHistoryBufferTextureIndex + 1) % m_HistoryBufferSize;
		m_AAFrameBuffer->SetAttachment(0, m_HistoryBuffer[m_ActiveHistoryBufferTextureIndex]);
	}

	m_Renderer->BeginRenderPass("TAA", m_AAFramebuffer, m_TAAShader);

	m_Context->SetShaderDataTexture("u_PreviousColor", m_HistoryBuffer[(m_ActiveHistoryBufferTextureIndex - 1 + m_HistoryBufferSize) % m_HistoryBufferSize]);
	m_Context->SetShaderDataTexture("u_CurrentColor", m_Renderer->GetRenderTarget(RenderTarget::Light));
	m_Context->SetShaderDataTexture("u_CurrentDepth", m_Renderer->GetRenderTarget(RenderTarget::GDepth));
	m_Context->SetShaderDataTexture("u_Velocity", m_Renderer->GetRenderTarget(RenderTarget::GVelocity));
	m_Context->SetShaderDataFloat2("u_PixelSize", 1.0f / glm::vec2(m_AAFramebuffer->GetWidth(), m_AAFramebuffer->GetHeight()));
	m_Context->SetShaderDataFloat2("u_ScreenSize", glm::vec2(m_AAFramebuffer->GetWidth(), m_AAFramebuffer->GetHeight()));
	m_Context->SetShaderDataFloat("u_Gamma", m_TAAGamma);

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);
	
	m_Renderer->EndRenderPass();
}

void TAARenderTask::Resize(glm::ivec2 size, float upscale)
{
	{
		int32_t previous = (m_ActiveHistoryBufferTextureIndex - 1 + m_HistoryBufferSize) % m_HistoryBufferSize;
		std::shared_ptr<Framebuffer> framebuffer = std::static_pointer_cast<Framebuffer>(m_HistoryBuffer[previous]);
		framebuffer->Resize(size.x * upscale, size.y * upscale);
	}
}
