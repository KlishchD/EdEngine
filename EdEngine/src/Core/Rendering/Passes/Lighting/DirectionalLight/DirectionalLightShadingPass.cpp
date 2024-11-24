#include "DirectionalLightShadingPass.h"

void DirectionalLightShadingRenderPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<DirecationalLightShadingParameters, DirectionalLightShadingShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Directional light pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\light\\directional-light-pass.glsl");

	m_Parameters.SourceFactor = BlendFactor::One;
	m_Parameters.DestinationFactor = BlendFactor::One;

	m_ShaderParameters.Albedo = m_Parameters.Albedo;
	m_ShaderParameters.Position = m_Parameters.Position;
	m_ShaderParameters.Normal = m_Parameters.Normal;
	m_ShaderParameters.RoughnessMetalic = m_Parameters.RoughnessMetalic;

	m_ShaderParameters.Light_ShadowMap = m_Parameters.ShadowMap;
}

void DirectionalLightShadingRenderPass::Execute()
{
	RenderPass<DirecationalLightShadingParameters, DirectionalLightShadingShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	m_Renderer->SetCamera(m_Parameters.Camera->GetCamera());

	std::shared_ptr<DirectionalLightComponent> light = m_Parameters.Light;

	m_ShaderParameters.FarPlane = m_Renderer->GetFarPlane();
	m_ShaderParameters.PixelSize = 1.0f / glm::vec2(m_Parameters.Albedo->GetSize());

	m_ShaderParameters.Light_Color = light->GetColor();
	m_ShaderParameters.Light_Intensity = light->GetIntensity();
	m_ShaderParameters.Light_Direction = light->GetDirection();

	m_ShaderParameters.Light_IsShadowCasting = light->IsShadowCasting();

	if (light->IsShadowCasting())
	{
		m_ShaderParameters.Light_ShadowMapPixelSize = 1.0f / m_Parameters.DrawFramebuffer->GetWidth();
		
		m_ShaderParameters.Light_ShadowFilterSize = light->GetShadowFilterSize();
		m_ShaderParameters.Light_ShadowFilterRadius = light->GetShadowFilterRadius();

		m_ShaderParameters.Light_Cascades = m_Parameters.ShadowViewProjectionMatrices->size();
		for (int32_t i = 0; i < m_ShaderParameters.Light_Cascades; ++i)
		{
			m_ShaderParameters.Light_ShadowProjectionViewMatries[i] = m_Parameters.ShadowViewProjectionMatrices.Get()[i];
		}
	}

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();

	m_Renderer->DirectionalLightsShaded++;
}
