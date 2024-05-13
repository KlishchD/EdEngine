#include "FXAAPass.h"

void FXAAPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<FXAAPassParameters, FXAAPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "FXAA pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\AA\\FXAA.glsl");

	m_Parameters.Output = RenderingHelper::CreateRenderTarget<Texture2D>({ "FXAA.Output", FramebufferAttachmentType::Color16 }, TextureType::Texture2D);

	m_ShaderParameters.Input = m_Parameters.LightCombined;

	m_ShaderParameters.ContrastThreshold = 0.0312f;
	m_ShaderParameters.RelativeThreshold = 0.125f;
	m_ShaderParameters.SubpixelBlending = 1.0f;
}

void FXAAPass::Execute()
{
	RenderPass<FXAAPassParameters, FXAAPassShaderParameters>::Execute();
	
	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.Output->Resize(size.x, size.y, 1);

	if (m_Renderer->GetAAMethod() == AAMethod::FXAA)
	{
		m_ShaderParameters.Output = m_Parameters.Output;

		m_ShaderParameters.PixelSize = glm::vec2(1.0f / m_Parameters.LightCombined->GetWidth(), 1.0f / m_Parameters.LightCombined->GetHeight());

		SubmitShaderParameters();

		m_Context->RunComputeShader(m_Parameters.Output->GetWidth(), m_Parameters.Output->GetHeight(), 1);

		m_Context->Barier(BarrierType::AllBits);
	}
}

void FXAAPass::SetContrastThreshold(float threshold)
{
	m_ShaderParameters.ContrastThreshold = threshold;
}

float FXAAPass::GetContrastThreshold() const
{
	return m_ShaderParameters.ContrastThreshold;
}

void FXAAPass::SetRelativeThreshold(float threshold)
{
	m_ShaderParameters.RelativeThreshold = threshold;
}

float FXAAPass::GetRelativeThreshold() const
{
	return m_ShaderParameters.RelativeThreshold;
}

void FXAAPass::SetSubpixelBlending(float scale)
{
	m_ShaderParameters.SubpixelBlending = scale;
}

float FXAAPass::GetSubpixelBlending() const
{
	return m_ShaderParameters.SubpixelBlending;
}