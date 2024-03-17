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

void SSAOBlurPass::Update(float deltaSeconds)
{
	RenderPass<SSAOBlurPassParameters, SSAOBlurPassShaderParameters>::Update(deltaSeconds);

	if (m_Renderer->IsViewportSizeDirty())
	{
		glm::u32vec2 size = m_Renderer->GetViewportSize();
		m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
	}

	m_ShaderParameters.PixelSize = glm::vec2(1.0f / m_Parameters.DrawFramebuffer->GetWidth(), 1.0f / m_Parameters.DrawFramebuffer->GetHeight());

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}
