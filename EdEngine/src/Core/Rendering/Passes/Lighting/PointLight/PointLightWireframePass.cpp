#include "PointLightWireframePass.h"

void PointLightWireframePass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<PointLightWireframePassParameters, PointLightWireframePassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Light wire frame pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\wireframe.glsl");

	m_Parameters.SourceFactor = BlendFactor::One;
	m_Parameters.DestinationFactor = BlendFactor::One;
}

void PointLightWireframePass::Execute()
{
	RenderPass<PointLightWireframePassParameters, PointLightWireframePassShaderParameters>::Execute();

	if (m_Renderer->IsViewportSizeDirty())
	{
		glm::u32vec2 size = m_Renderer->GetViewportSize();
		m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
	}

	if (m_Parameters.Light->ShouldShowWireframe())
	{
		m_Renderer->SetCamera(m_Parameters.Camera->GetCamera());

		std::shared_ptr<PointLightComponent> light = m_Parameters.Light;

		Transform transform = light->GetWorldTransform();
		transform.SetScale(glm::vec3(light->GetRadius()));

		m_ShaderParameters.ModelMatrix = transform.GetMatrix();

		SubmitShaderParameters();

		m_Context->SetVertexBuffer(m_Parameters.LightMeshVBO);
		m_Context->SetIndexBuffer(m_Parameters.LightMeshIBO);
		m_Context->Draw(DrawMode::LineStrip);
	}
}
