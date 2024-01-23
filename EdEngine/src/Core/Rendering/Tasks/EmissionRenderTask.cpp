#include "EmissionRenderTask.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"
#include "Utils/RenderingHelper.h"
#include "Utils/Files.h"

void EmissionRenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	m_EmissionPassShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\emission-pass.glsl)");
}

void EmissionRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	m_Renderer->BeginRenderPass("Light emission pass", m_Renderer->GetLightFramebuffer(), m_EmissionPassShader);

	m_Context->ClearColorTarget();
	m_Context->ClearDepthTarget();

	m_Context->SetShaderDataTexture("u_Albedo", m_Renderer->GetRenderTarget(RenderTarget::GAlbedo));
	m_Context->SetShaderDataTexture("u_RoughnessMetalic", m_Renderer->GetRenderTarget(RenderTarget::GRougnessMetalicEmission));

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();
}
