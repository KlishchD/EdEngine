#include "AmbientRenderTask.h"
#include "Utils/RenderingHelper.h"
#include "Utils/Files.h"

void AmbientRenderTask::Setup(Renderer* renderer)
{
	m_AmbientShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\ambient-pass.glsl)");
}

void AmbientRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	m_Renderer->BeginRenderPass("Ambient pass", m_Renderer->GetLightFramebuffer(), m_AmbientShader);

	m_Context->SetBlending(BlendFactor::One, BlendFactor::One);

	m_Context->SetShaderDataTexture("u_Albedo", m_Renderer->GetRenderTarget(RenderTarget::GAlbedo));
	
	if (m_Renderer->IsSSAOEnabled())
	{
		m_Context->SetShaderDataTexture("u_AmbientOcclusion", m_Renderer->GetRenderTarget(RenderTarget::SSAO));
	}
	else
	{
		m_Context->SetShaderDataTexture("u_AmbientOcclusion", RenderingHelper::GetWhiteTexture());
	}

	m_Renderer->EndRenderPass();
}
