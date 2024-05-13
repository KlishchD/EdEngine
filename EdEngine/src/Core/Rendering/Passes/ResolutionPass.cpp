#include "ResolutionPass.h"

void ResolutionPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<ResolutionPassParameters, ResolutionPassShaderParameters>::Initialize(graph);
	
	m_Parameters.Name = "Resolution pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\deferred\\resolution-pass.glsl");

	m_ShaderParameters.Gamma = 2.2f;
	m_ShaderParameters.BloomStrength = 0.1f;
	m_ShaderParameters.BloomIntensity = 1.0f;

	// todo: think if depth is necessary (it is just for icons)
}

void ResolutionPass::Execute()
{
	RenderPass<ResolutionPassParameters, ResolutionPassShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
	
	switch (m_Renderer->GetAAMethod())
	{
		case AAMethod::TAA: m_ShaderParameters.Light = m_Parameters.TAAOutput; break;
		case AAMethod::FXAA: m_ShaderParameters.Light = m_Parameters.FXAAOutput; break;
		default: m_ShaderParameters.Light = m_Parameters.Light; break;
	}

	m_ShaderParameters.IsBloomEnabled = m_Renderer->IsBloomEnabled();
	m_ShaderParameters.Bloom = m_Parameters.Bloom;

	SubmitShaderParameters();
	
	m_Renderer->SubmitFullScreenQuad();
}

void ResolutionPass::SetGamma(float gamma)
{
	m_ShaderParameters.Gamma = gamma;
}

float ResolutionPass::GetGamma() const
{
	return m_ShaderParameters.Gamma;
}

void ResolutionPass::SetBloomStrength(float strength)
{
	m_ShaderParameters.BloomStrength = strength;
}

float ResolutionPass::GetBloomStrength() const
{
	return m_ShaderParameters.BloomStrength;
}

void ResolutionPass::SetBloomIntensity(float intensity)
{
	m_ShaderParameters.BloomIntensity = intensity;
}

float ResolutionPass::GetBloomIntensity() const
{
	return m_ShaderParameters.BloomIntensity;
}
