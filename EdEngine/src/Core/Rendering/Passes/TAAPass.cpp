#include "TAAPass.h"

void TAAPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<TAAPassParameters, TAAPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "TAA Pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\AA\\TAA.glsl");

	m_HistoryBuffer = RenderingHelper::CreateTexture2D("History buffer texture");
	m_HistoryBuffer->SetWrapS(WrapMode::ClampToEdge);
	m_HistoryBuffer->SetWrapT(WrapMode::ClampToEdge);
	m_HistoryBuffer->SetPixelFormat(PixelFormat::RGBA16F);

	m_HistoryBuffer->Initialize();

	m_ShaderParameters.Gamma = 1.25f;
}

void TAAPass::PreExecute()
{
	if (m_Renderer->GetAAMethod() == AAMethod::TAA)
	{
		std::shared_ptr<Texture2D> newHistoryBuffer = m_Parameters.DrawFramebuffer->GetAttachment<Texture2D>(0);
		
		m_Parameters.DrawFramebuffer->SetAttachment(0, m_HistoryBuffer, FramebufferSizeAdjustmentMode::ResizeTextureToFramebufferSize);
		m_Parameters.Output = m_HistoryBuffer;

		m_HistoryBuffer = newHistoryBuffer;
	}

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
	m_HistoryBuffer->Resize(size.x, size.y, 1);
}

void TAAPass::Execute()
{
	RenderPass<TAAPassParameters, TAAPassShaderParameters>::Execute();

	if (m_Renderer->GetAAMethod() == AAMethod::TAA)
	{
		m_ShaderParameters.PreviousColor = m_HistoryBuffer;
		m_ShaderParameters.CurrentColor = m_Parameters.SceneBase;
		m_ShaderParameters.CurrentDepth = m_Parameters.SceneDepth;
		m_ShaderParameters.Velocity = m_Parameters.SceneVelocity;
		
		m_ShaderParameters.PixelSize = 1.0f / glm::vec2(m_HistoryBuffer->GetSize());
		m_ShaderParameters.ScreenSize = glm::vec2(m_HistoryBuffer->GetSize());

		SubmitShaderParameters();

		m_Renderer->SubmitFullScreenQuad();
	}
}

void TAAPass::SetGamma(float gamma)
{
	m_ShaderParameters.Gamma = gamma;
}

float TAAPass::GetGamma() const
{
	return m_ShaderParameters.Gamma;
}