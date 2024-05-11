#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/CameraComponent.h"
#include "Core/Components/StaticMeshComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(GBufferPass, Base)
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Albedo,           Color,     "GBuffer.Albedo")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Position,         Position,  "GBuffer.Position")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Normal,           Direction, "GBuffer.Normal")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, RoughnessMetalic, Color16,   "GBuffer.RoughnessMetalic")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Velocity,         Velocity,  "GBuffer.Velocity")
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Depth,            Depth,     "GBuffer.Depth")

	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera", Read)

	ED_RENDER_PASS_PARAMETER(std::vector<std::shared_ptr<StaticMeshComponent>>, Meshes, "Scene.StaticMesh", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(GBufferPass)

	ED_SHADER_PARAMETER_SUBSTRUCT(Material, Float3, glm::vec3, BaseColor)
	
	ED_SHADER_PARAMETER_SUBSTRUCT(Material, Float, float, Roughness)
	ED_SHADER_PARAMETER_SUBSTRUCT(Material, Float, float, Metalic)
	ED_SHADER_PARAMETER_SUBSTRUCT(Material, Float, float, Emission)

	ED_SHADER_PARAMETER_SUBSTRUCT_PTR(Material, Texture, Texture2D, BaseColorTexture)

	ED_SHADER_PARAMETER_SUBSTRUCT_PTR(Material, Texture, Texture2D, NormalTexture)
	ED_SHADER_PARAMETER_SUBSTRUCT(Material, Bool, bool, PerformNormalMapping)

	ED_SHADER_PARAMETER_SUBSTRUCT_PTR(Material, Texture, Texture2D, RoughnessTexture)
	ED_SHADER_PARAMETER_SUBSTRUCT_PTR(Material, Texture, Texture2D, MetalicTexture)

	ED_SHADER_PARAMETER(Mat4, glm::mat4, PreviousModelMatrix)
	ED_SHADER_PARAMETER(Mat4, glm::mat4, ModelMatrix)
	ED_SHADER_PARAMETER(Mat3, glm::mat3, NormalMatrix)

ED_END_SHADER_PARAMETERS_DECLARATION()

class GBufferPass : public RenderPass<GBufferPassParameters, GBufferPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph);
	virtual void Execute();

protected:
	void SetCameraInformation();

	inline void SetTextureOrWhite(std::shared_ptr<Texture2D>& destination, std::shared_ptr<Texture2D> texture)
	{
		destination = texture ? texture : RenderingHelper::GetWhiteTexture();
	}

protected:
	int32_t m_JitterSequenceSize = 16;
	int32_t m_CurrentJitterIndex = 0;
	std::vector<glm::vec2> m_JitterSequence;

	glm::mat4 m_PreviousView;
};