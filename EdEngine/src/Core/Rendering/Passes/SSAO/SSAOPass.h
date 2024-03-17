#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/CameraComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(SSAORenderPassParameters, Base)
	
	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Base, Distance, "SSAO.Base")

	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Position, "GBuffer.Position")
	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, Normal, "GBuffer.Normal")

	ED_RENDER_PASS_GRAPH_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(SSAORenderPassShaderParameters)

	ED_SHADER_PARAMETER(Mat4, glm::mat4, NormalMatrix)

	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Position)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Normal)

	ED_SHADER_PARAMETER(Float, float, SampleCount)

	ED_SHADER_PARAMETER(Float, float, NoiseSize)
	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Noise)

	ED_SHADER_PARAMETER_ARRAY(Float3, glm::vec3, Samples, 32)

ED_END_SHADER_PARAMETERS_DECLARATION()

class SSAOPass : public RenderPass<SSAORenderPassParameters, SSAORenderPassShaderParameters>
{
	static const uint32_t MaxSSAOSamplesCount = 32;
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;
};