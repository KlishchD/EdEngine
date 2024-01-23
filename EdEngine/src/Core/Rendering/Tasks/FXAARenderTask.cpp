#include "FXAARenderTask.h"
#include "Utils/Files.h"
#include "Utils/RenderingHelper.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"

void FXAARenderTask::Setup(Renderer* renderer)
{
	m_FFXAShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\AA\FXAA.glsl)");
	
}

void FXAARenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	m_Context->SetShader(m_FFXAShader);

	std::shared_ptr<Texture2D> input = m_Renderer->GetRenderTarget(RenderTarget::Light);

	m_Context->SetShaderDataTexture("u_Input", input);
	m_Context->SetShaderDataImage("u_Output", m_AAFramebuffer->GetAttachment(0));
	m_Context->SetShaderDataFloat("u_ContrastThreshold", m_ContrastThreshold);
	m_Context->SetShaderDataFloat("u_RelativeThreshold", m_RelativeThreshold);
	m_Context->SetShaderDataFloat("u_SubpixelBlending", m_SubpixelBlending);

	glm::vec2 size = glm::vec2(1.0f / input->GetWidth(), 1.0f / input->GetHeight());
	m_Context->SetShaderDataFloat2("u_PixelSize", size);

	m_Context->RunComputeShader(m_AAFramebuffer->GetWidth(), m_AAFramebuffer->GetHeight(), 1);

	m_Context->Barier(BarrierType::AllBits);
}

void FXAARenderTask::SetContrastThreshold(float threshold)
{
	m_ContrastThreshold = threshold;
}

float FXAARenderTask::GetContrastThreshold() const
{
	return m_ContrastThreshold;
}

void FXAARenderTask::SetRelativeThreshold(float threshold)
{
	m_RelativeThreshold = threshold;
}

float FXAARenderTask::GetRelativeThreshold() const
{
	return m_RelativeThreshold;
}

void FXAARenderTask::SetSubpixelBlending(float scale)
{
	m_SubpixelBlending = scale;
}

float FXAARenderTask::GetSubpixelBlending() const
{
	return m_SubpixelBlending;
}
