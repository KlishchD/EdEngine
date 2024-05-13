#include "SpotLightWireframePass.h"

void SpotLightWireframePass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<SpotLightWireframePassParameters, SpotLightWireframePassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Spot wire frame light pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\wireframe.glsl");

	m_Parameters.SourceFactor = BlendFactor::One;
	m_Parameters.DestinationFactor = BlendFactor::OneMinusSourceAlpha;
}

void SpotLightWireframePass::Execute()
{
	RenderPass<SpotLightWireframePassParameters, SpotLightWireframePassShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	std::shared_ptr<SpotLightComponent> light = m_Parameters.Light;

	if (light->ShouldShowWireframe())
	{
		m_Renderer->SetCamera(m_Parameters.Camera->GetCamera());

		const float angle = light->GetOuterAngle();
		const float length = light->GetMaxDistance();
		const float radius = glm::tan(angle) * length;

		Transform transform = light->GetWorldTransform();
		transform.SetScale(glm::vec3(radius, length, radius));

		m_ShaderParameters.ModelMatrix = transform.GetMatrix();

		SubmitShaderParameters();

		m_Context->SetVertexBuffer(m_Parameters.LightMeshVBO);
		m_Context->SetIndexBuffer(m_Parameters.LightMeshIBO);
		m_Context->Draw(DrawMode::LineStrip);
	}
}
