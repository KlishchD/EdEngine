#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/CameraComponent.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Rendering/Textures/CubeTexture.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(PointLightShadingPassParameters, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Diffuse, "LightBuffer.Diffuse")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Specular, "LightBuffer.Specular")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Combined, "LightBuffer.Combined")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Albedo, "GBuffer.Albedo")
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Position, "GBuffer.Position")
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Normal, "GBuffer.Normal")
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, RoughnessMetalic, "GBuffer.RoughnessMetalic")
	ED_RENDER_PASS_RESOURCE_REFERENCE(CubeTexture, ShadowMap, "PointLightPass.ShadowMap")

	ED_RENDER_PASS_GRAPH_PARAMETER(std::vector<glm::vec3>, LightMeshVertices, "PointLightPass.LightMeshVertices")
	ED_RENDER_PASS_GRAPH_PARAMETER(std::vector<int32_t>, LightMeshIndices, "PointLightPass.LightMeshIndices")

	ED_RENDER_PASS_GRAPH_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera")
	ED_RENDER_PASS_GRAPH_OBJECT_PTR_PARAMETER(PointLightComponent, Light, "PointLightPass.Light")

	ED_RENDER_PASS_DECLARE_RESOURCE(VertexBuffer, LightMeshVBO, "PointLightPass.LightMeshVBO")
	ED_RENDER_PASS_DECLARE_RESOURCE(IndexBuffer, LightMeshIBO, "PointLightPass.LightMeshIBO")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(PointLightShadingShaderParameters)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Albedo)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Position)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Normal)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, RoughnessMetalic)

	ED_SHADER_PARAMETER(Float, float, FarPlane)
	ED_SHADER_PARAMETER(Float2, glm::vec2, PixelSize)

	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float3, glm::vec3, Position)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float3, glm::vec3, Color)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, Intensity)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, Radius)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Bool, bool, UseShadowMap)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, ShadowMapPixelSize)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, FilterSize)
	ED_SHADER_PARAMETER_SUBSTRUCT_PTR(Light, Texture, CubeTexture, ShadowMap)

	ED_SHADER_PARAMETER(Mat4, glm::mat4, ModelMatrix)

ED_END_SHADER_PARAMETERS_DECLARATION()

class PointLightShadingPass : public RenderPass<PointLightShadingPassParameters, PointLightShadingShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;
};