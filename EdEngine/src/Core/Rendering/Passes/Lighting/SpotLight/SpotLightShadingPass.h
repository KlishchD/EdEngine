#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/SpotLightComponent.h"
#include "Core/Components/CameraComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(SpotLightShading, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Diffuse,  "LightBuffer.Diffuse")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Specular, "LightBuffer.Specular")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Combined, "LightBuffer.Combined")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Albedo,           "GBuffer.Albedo",           Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Position,         "GBuffer.Position",         Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Normal,           "GBuffer.Normal",           Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, RoughnessMetalic, "GBuffer.RoughnessMetalic", Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, ShadowMap,        "SpotLightPass.ShadowMap",  Read)

	ED_RENDER_PASS_PARAMETER(glm::mat4,              ShadowProjectionViewMatrix, "SpotLightPass.ShadowProjectionViewMatrix", Read)
	ED_RENDER_PASS_PARAMETER(std::vector<glm::vec3>, LightMeshVertices,          "SpotLightPass.LightMeshVertices",          Read)
	ED_RENDER_PASS_PARAMETER(std::vector<int32_t>,   LightMeshIndices,           "SpotLightPass.LightMeshIndices",           Read)

	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent,    Camera, "Camera"             , Read)
	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(SpotLightComponent, Light,  "SpotLightPass.Light", Read)

	ED_RENDER_PASS_DECLARE_RESOURCE(VertexBuffer, LightMeshVBO, "SpotLightPass.LightMeshVBO")
	ED_RENDER_PASS_DECLARE_RESOURCE(IndexBuffer,  LightMeshIBO, "SpotLightPass.LightMeshIBO")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(SpotLightShading)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Albedo)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Position)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Normal)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, RoughnessMetalic)

	ED_SHADER_PARAMETER(Float, float, FarPlane)
	ED_SHADER_PARAMETER(Float2, glm::vec2, PixelSize)

	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float3, glm::vec3, Color)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, MaxDistance)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, Intensity)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float3, glm::vec3, Position)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, InnerAngleCos)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, OuterAngleCos)

	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Bool, bool, IsShadowCasting)
	ED_SHADER_PARAMETER_SUBSTRUCT_PTR(Light, Texture, Texture2D, ShadowMap)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float2, glm::vec2, ShadowMapPixelSize)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Mat4, glm::mat4, ShadowProjectionViewMatrix)
	
	ED_SHADER_PARAMETER_SUBSTRUCT_PTR(Light, Texture, Texture2D, ShadowSamples)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float2, glm::vec2, ShadowSamplesPixelSize)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, ShadowFilterSize)
	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float, float, ShadowFilterRadius)

	ED_SHADER_PARAMETER_SUBSTRUCT(Light, Float3, glm::vec3, Direction)

	ED_SHADER_PARAMETER(Mat4, glm::mat4, ModelMatrix)

ED_END_SHADER_PARAMETERS_DECLARATION()


class SpotLightShadingPass : public RenderPass<SpotLightShadingParameters, SpotLightShadingShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;

	void SetShadowSamplesBlockCount(uint32_t count);
	uint32_t GetShadowSamplesBlocksCount() const;

	void SetShadowSamplesBlockSize(uint32_t size);
	uint32_t GetShadowSamplesBlockSize() const;

protected:
	void UpdateShadowSamplesTexture();

protected:
	uint32_t m_ShadowSamplesBlockCount = 10;
	uint32_t m_ShadowSamplesBlockSize = 32;
};