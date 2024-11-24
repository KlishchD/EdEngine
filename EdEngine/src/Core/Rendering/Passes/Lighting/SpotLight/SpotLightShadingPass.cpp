#include "SpotLightShadingPass.h"
#include "Core/Rendering/Buffers/VertexBufferLayout.h"
#include "Helpers/MathHelper.h"

void SpotLightShadingPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<SpotLightShadingParameters, SpotLightShadingShaderParameters>::Initialize(graph);

	{
		std::shared_ptr<Texture2D> texture = RenderingHelper::CreateTexture2D("Spot light shadow samples");

		texture->SetPixelFormat(PixelFormat::RG32F);
		texture->SetFilteringMode(FilteringMode::Nearest);

		texture->Initialize();

		m_ShaderParameters.Light_ShadowSamples = texture;

		UpdateShadowSamplesTexture();
	}

	{
		std::vector<glm::vec3>& vertices = m_Parameters.LightMeshVertices;
		std::vector<int32_t>& indices = m_Parameters.LightMeshIndices;

		VertexBufferLayout lightVBOLayout = { { "position", ShaderDataType::Float3 } };
		m_Parameters.LightMeshVBO = RenderingHelper::CreateVertexBuffer(vertices.data(), 3.0f * sizeof(float) * vertices.size(), lightVBOLayout, BufferUsage::StaticDraw);
		m_Parameters.LightMeshIBO = RenderingHelper::CreateIndexBuffer(indices.data(), sizeof(int32_t) * indices.size(), BufferUsage::StaticDraw);
	}

	m_Parameters.Name = "Spot light pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\light\\spot-light-pass.glsl");

	m_Parameters.SourceFactor = BlendFactor::One;
	m_Parameters.DestinationFactor = BlendFactor::One;

	m_Parameters.bEnableFaceCulling = true;
	m_Parameters.FaceToCull = Face::Front;

	m_ShaderParameters.Albedo = m_Parameters.Albedo;
	m_ShaderParameters.Position = m_Parameters.Position;
	m_ShaderParameters.Normal = m_Parameters.Normal;
	m_ShaderParameters.RoughnessMetalic = m_Parameters.RoughnessMetalic;
}

void SpotLightShadingPass::Execute()
{
	RenderPass<SpotLightShadingParameters, SpotLightShadingShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	m_Renderer->SetCamera(m_Parameters.Camera->GetCamera());

	m_ShaderParameters.FarPlane = m_Renderer->GetFarPlane();
	m_ShaderParameters.PixelSize = 1.0f / glm::vec2(m_Parameters.Albedo->GetSize());

	std::shared_ptr<SpotLightComponent> light = m_Parameters.Light;

	m_ShaderParameters.Light_Color = light->GetColor();
	m_ShaderParameters.Light_MaxDistance = light->GetMaxDistance();
	m_ShaderParameters.Light_Intensity = light->GetIntensity();
	m_ShaderParameters.Light_Position = light->GetPosition();

	m_ShaderParameters.Light_InnerAngleCos = glm::cos(light->GetInnerAngle());
	m_ShaderParameters.Light_OuterAngleCos = glm::cos(light->GetOuterAngle());

	m_ShaderParameters.Light_IsShadowCasting = light->IsShadowCasting();
	m_ShaderParameters.Light_ShadowMap = m_ShaderParameters.Light_IsShadowCasting ? m_Parameters.ShadowMap.Get() : RenderingHelper::GetWhiteTexture();

	if (m_ShaderParameters.Light_IsShadowCasting)
	{
		m_ShaderParameters.Light_ShadowMapPixelSize = glm::vec2(1.0f / m_ShaderParameters.Light_ShadowMap->GetWidth(), 1.0f / m_ShaderParameters.Light_ShadowMap->GetHeight());

		m_ShaderParameters.Light_ShadowProjectionViewMatrix = m_Parameters.ShadowProjectionViewMatrix;

		m_ShaderParameters.Light_ShadowFilterSize = light->GetShadowFilterSize();
		m_ShaderParameters.Light_ShadowFilterRadius = light->GetShadowFilterRadius();
	}

	const float angle = light->GetOuterAngle();
	const float length = light->GetMaxDistance();
	const float radius = glm::tan(angle) * length;

	Transform transform = light->GetWorldTransform();
	transform.SetScale(glm::vec3(radius, length, radius));

	static const glm::vec3 spotLightMeshDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	m_ShaderParameters.Light_Direction = glm::normalize(transform.GetRotation() * spotLightMeshDirection);
	m_ShaderParameters.ModelMatrix = transform.GetMatrix();

	SubmitShaderParameters();

	m_Context->SetVertexBuffer(m_Parameters.LightMeshVBO);
	m_Context->SetIndexBuffer(m_Parameters.LightMeshIBO);
	m_Context->Draw();

	m_Renderer->SpotLightsShaded++;
}

void SpotLightShadingPass::SetShadowSamplesBlockCount(uint32_t count)
{
	m_ShadowSamplesBlockCount = count;
	UpdateShadowSamplesTexture();
}

uint32_t SpotLightShadingPass::GetShadowSamplesBlocksCount() const
{
	return m_ShadowSamplesBlockCount;
}

void SpotLightShadingPass::SetShadowSamplesBlockSize(uint32_t size)
{
	m_ShadowSamplesBlockSize = size;
	UpdateShadowSamplesTexture();
}

uint32_t SpotLightShadingPass::GetShadowSamplesBlockSize() const
{
	return m_ShadowSamplesBlockSize;
}

void SpotLightShadingPass::UpdateShadowSamplesTexture()
{
	std::vector<glm::vec2> samples = MathHelper::GenerateCircleSamples(m_ShadowSamplesBlockSize * m_ShadowSamplesBlockSize * m_ShadowSamplesBlockCount);
	Texture2DData data(m_ShadowSamplesBlockSize * m_ShadowSamplesBlockCount, m_ShadowSamplesBlockSize, (uint8_t*) &samples[0], samples.size() * sizeof(glm::vec2), false);

	m_ShaderParameters.Light_ShadowSamples->SetData(std::move(data));
	m_ShaderParameters.Light_ShadowSamplesPixelSize = glm::vec2(1.0f / (m_ShadowSamplesBlockSize * m_ShadowSamplesBlockCount), 1.0f / m_ShadowSamplesBlockSize);
}
