#include "EmissionPass.h"

void EmissionPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<EmissionPassParameters, EmissionPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Emission pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\deferred\\emission-pass.glsl");

	m_Parameters.DestinationFactor = BlendFactor::One;
	m_Parameters.SourceFactor = BlendFactor::One;

	m_ShaderParameters.Albedo = m_Parameters.Albedo;
	m_ShaderParameters.RoughnessMetalic = m_Parameters.RoughnessMetalic;
}

void EmissionPass::Execute()
{
	RenderPass<EmissionPassParameters, EmissionPassShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	SubmitShaderParameters();

	m_ShaderParameters.PixelSize = 1.0f / glm::vec2(m_Parameters.Diffuse->GetSize());

	m_Renderer->SubmitFullScreenQuad();
}
