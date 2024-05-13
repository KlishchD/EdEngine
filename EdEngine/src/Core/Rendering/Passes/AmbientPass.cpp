#include "AmbientPass.h"
#include "Core/Rendering/Framebuffer.h"

void AmbientPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<AmbientPassParameters, AmbientPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Ambient pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\deferred\\ambient-pass.glsl");
	
	m_Parameters.bClearColors = true;
	m_Parameters.bClearDepth = true;

	m_ShaderParameters.Albedo = m_Parameters.Albedo;
}

void AmbientPass::Execute()
{
	RenderPass<AmbientPassParameters, AmbientPassShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	m_ShaderParameters.AmbientOcclusion = m_Renderer->IsSSAOEnabled() ? m_Parameters.AmbientOcclusion.Get() : RenderingHelper::GetWhiteTexture();

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}
