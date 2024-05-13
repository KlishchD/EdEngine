#include "SSAOBlurPass.h"

void SSAOBlurPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<SSAOBlurPassParameters, SSAOBlurPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "SSAO blur";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\SSAOBlur.glsl");

	m_Parameters.bUseBlending = false;
	m_Parameters.bClearColors = false;

	m_ShaderParameters.AmbientOcclusion = m_Parameters.AmbientOcclusion;
}

void SSAOBlurPass::Execute()
{
	RenderPass<SSAOBlurPassParameters, SSAOBlurPassShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(std::max<int32_t>(size.x / 2, 1), std::max<int32_t>(size.y / 2, 1), 1);

	m_ShaderParameters.PixelSize = glm::vec2(1.0f / m_Parameters.DrawFramebuffer->GetWidth(), 1.0f / m_Parameters.DrawFramebuffer->GetHeight());

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}
