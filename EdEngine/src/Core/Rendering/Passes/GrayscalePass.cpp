#include "GrayscalePass.h"

void GrayscalePass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<GrayscalePassParameters, GrayscalePassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Grayscale pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\Grayscale.glsl");;
	m_ShaderParameters.Color = m_Parameters.Color;
}

void GrayscalePass::Execute()
{
	RenderPass<GrayscalePassParameters, GrayscalePassShaderParameters>::Execute();

	return;

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}
