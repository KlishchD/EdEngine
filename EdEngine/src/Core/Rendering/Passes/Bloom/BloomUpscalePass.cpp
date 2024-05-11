#include "BloomUpscalePass.h"

void BloomUpscalePass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<BloomUpscalePassParameters, BloomUpscalePassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Bloom upscale";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\bloom\\upscale.glsl");

	m_ShaderParameters.MixStrength = 0.85f;
}

void BloomUpscalePass::Execute()
{
	RenderPass<BloomUpscalePassParameters, BloomUpscalePassShaderParameters>::Execute();

	glm::u32vec3 size = m_Parameters.Upscaled->GetSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, size.z);

	m_ShaderParameters.Downscaled = m_Parameters.Downscaled;
	m_ShaderParameters.Upscaled = m_Parameters.Upscaled; // TODO: Should probably change this :) (reading and writing to this target at this pass)

	m_ShaderParameters.DownscaledPixelSize = 1.0f / glm::vec2(m_ShaderParameters.Downscaled->GetSize());
	m_ShaderParameters.UpscaledPixelSize = 1.0f / glm::vec2(m_ShaderParameters.Upscaled->GetSize());

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}

float BloomUpscalePass::GetBloomMixStrength() const
{ 
	return m_ShaderParameters.MixStrength;
}

void BloomUpscalePass::SetBloomMixStrength(float strength)
{
	m_ShaderParameters.MixStrength = strength;
}