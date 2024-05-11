#include "BloomDownscalePass.h"

void BloomDownscalePass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<BloomDownscalePassParameters, BloomDownscalePassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Bloom downscale";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\bloom\\downscale.glsl");
}

void BloomDownscalePass::Execute()
{
	RenderPass<BloomDownscalePassParameters, BloomDownscalePassShaderParameters>::Execute();
	
	if (m_Parameters.InstanceNumber == 1)
	{
		switch (m_Renderer->GetAAMethod())
		{
		case AAMethod::TAA: m_ShaderParameters.Input = m_Parameters.TAAOutput; break;
		case AAMethod::FXAA: m_ShaderParameters.Input = m_Parameters.FXAAOutput; break;
		default: m_ShaderParameters.Input = m_Parameters.Scene; break;
		}
	}
	else
	{
		m_ShaderParameters.Input =  m_Parameters.Previous;
	}

	glm::vec2 inputSize = glm::vec2(m_ShaderParameters.Input->GetSize());

	glm::vec2 outputSize = inputSize / 2.0f;
	m_Parameters.DrawFramebuffer->Resize(outputSize.x, outputSize.y, 1);

	m_ShaderParameters.InPixelSize = 1.0f / inputSize;
	m_ShaderParameters.OutPixelSize = 1.0f / outputSize;

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}

std::shared_ptr<Texture2D> BloomDownscalePass::GetTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_Parameters.DrawFramebuffer->GetAttachment(0));
}

std::shared_ptr<Framebuffer> BloomDownscalePass::GetFramebuffer() const
{
	return m_Parameters.DrawFramebuffer;
}
