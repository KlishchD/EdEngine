#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/CameraComponent.h"
#include "Core/Components/DirectionalLightComponent.h"
#include "Core/Rendering/Textures/Texture2DArray.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(DirecationalLightShading, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Diffuse,  "LightBuffer.Diffuse")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Specular, "LightBuffer.Specular")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Combined, "LightBuffer.Combined")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Albedo,           "GBuffer.Albedo",           Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Position,         "GBuffer.Position",         Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Normal,           "GBuffer.Normal",           Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, RoughnessMetalic, "GBuffer.RoughnessMetalic", Read)

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2DArray, ShadowMap, "DirectionalLightPass.ShadowMap", Read)

	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(DirectionalLightComponent, Light, "DirectionalLightPass.Light", Read)
	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera", Read)

	ED_RENDER_PASS_PARAMETER(std::vector<glm::mat4>, ShadowViewProjectionMatrices, "DirectionalLightPass.ShadowViewProjectionMatrices", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(DirectionalLightShading)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Albedo)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Position)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Normal)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, RoughnessMetalic)

	ED_SHADER_PARAMETER(Float, float, FarPlane)
	ED_SHADER_PARAMETER(Float2, glm::vec2, PixelSize)

	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float3, glm::vec3, Color)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, Intensity)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float3, glm::vec3, Direction)

	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Bool, bool, IsShadowCasting)

	ED_SHADER_PARAMETER_SUBSTRUCT_PTR(Light, Texture, Texture2DArray, ShadowMap)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, ShadowMapPixelSize)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, ShadowFilterSize)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, ShadowFilterRadius)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, Cascades)

	ED_SHADER_PARAMETER_SUBSTRUCT_ARRAY(Light, Mat4, glm::mat4, ShadowProjectionViewMatries, 4)

ED_END_SHADER_PARAMETERS_DECLARATION()

class DirectionalLightShadingRenderPass : public RenderPass<DirecationalLightShadingParameters, DirectionalLightShadingShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;

};