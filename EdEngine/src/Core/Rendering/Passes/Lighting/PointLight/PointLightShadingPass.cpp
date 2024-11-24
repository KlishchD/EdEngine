#include "PointLightShadingPass.h"
#include "Core/Rendering/Buffers/VertexBufferLayout.h"

void PointLightShadingPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<PointLightShadingPassParameters, PointLightShadingShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Point light pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\light\\point-light-pass.glsl");

	{
		std::vector<glm::vec3>& vertices = m_Parameters.LightMeshVertices;
		std::vector<int32_t>& indices = m_Parameters.LightMeshIndices;

		VertexBufferLayout lightVBOLayout = { { "position", ShaderDataType::Float3 } };
		m_Parameters.LightMeshVBO = RenderingHelper::CreateVertexBuffer(vertices.data(), 3.0f * sizeof(float) * vertices.size(), lightVBOLayout, BufferUsage::StaticDraw);
		m_Parameters.LightMeshIBO = RenderingHelper::CreateIndexBuffer(indices.data(), sizeof(int32_t) * indices.size(), BufferUsage::StaticDraw);
	}

	m_Parameters.SourceFactor = BlendFactor::One;
	m_Parameters.DestinationFactor = BlendFactor::One;

	m_Parameters.bEnableFaceCulling = true;
	m_Parameters.FaceToCull = Face::Front;

	m_ShaderParameters.Albedo = m_Parameters.Albedo;
	m_ShaderParameters.Position = m_Parameters.Position;
	m_ShaderParameters.Normal = m_Parameters.Normal;
	m_ShaderParameters.RoughnessMetalic = m_Parameters.RoughnessMetalic;
}

void PointLightShadingPass::Execute()
{
	RenderPass<PointLightShadingPassParameters, PointLightShadingShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	std::shared_ptr<PointLightComponent> light = m_Parameters.Light;
	
	m_Renderer->SetCamera(m_Parameters.Camera->GetCamera());

	m_ShaderParameters.FarPlane = m_Renderer->GetFarPlane();
	m_ShaderParameters.PixelSize = 1.0f / glm::vec2(m_Parameters.Albedo->GetSize());

	m_ShaderParameters.Light_Position = light->GetPosition();
	m_ShaderParameters.Light_Color = light->GetColor();
	m_ShaderParameters.Light_Intensity = light->GetIntensity();
	m_ShaderParameters.Light_Radius = light->GetRadius();

	m_ShaderParameters.Light_UseShadowMap = light->IsShadowCasting();
	m_ShaderParameters.Light_ShadowMap = m_Parameters.ShadowMap.Get();
	m_ShaderParameters.Light_FilterSize = light->GetShadowFilterSize();
	m_ShaderParameters.Light_ShadowMapPixelSize = 1.0f / m_Parameters.DrawFramebuffer->GetWidth();

	Transform transform = light->GetWorldTransform();
	transform.SetScale(glm::vec3(light->GetRadius()));
	
	m_ShaderParameters.ModelMatrix = transform.GetMatrix();

	SubmitShaderParameters();

	m_Context->SetVertexBuffer(m_Parameters.LightMeshVBO);
	m_Context->SetIndexBuffer(m_Parameters.LightMeshIBO);
	m_Context->Draw();

	m_Renderer->PointLightsShaded++;
}
