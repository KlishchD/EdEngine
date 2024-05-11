#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/DirectionalLightComponent.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/CameraComponent.h"
#include "Core/Rendering/Textures/Texture2DArray.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(DirectionalLightShadowPass, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2DArray, ShadowMap, Depth, "DirectionalLightPass.ShadowMap")

	ED_RENDER_PASS_PARAMETER(std::vector<std::shared_ptr<StaticMeshComponent>>, Meshes, "Scene.StaticMesh", Read)

	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(DirectionalLightComponent, Light, "DirectionalLightPass.Light", Read)
	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera", Read)

	ED_RENDER_PASS_DECLARE_PARAMETER(std::vector<glm::mat4>, ShadowViewProjectionMatrices, "DirectionalLightPass.ShadowViewProjectionMatrices")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(DirectionalLightShadowPass)

	ED_SHADER_PARAMETER(Float, float, Cascades)
	ED_SHADER_PARAMETER_ARRAY(Mat4, glm::mat4, ProjectionViewMatries, 4)

	ED_SHADER_PARAMETER(Mat4, glm::mat4, ModelMatrix)

ED_END_SHADER_PARAMETERS_DECLARATION()

class DirectionalLightShadowPass : public RenderPass<DirectionalLightShadowPassParameters, DirectionalLightShadowPassShaderParameters>
{
	static const uint32_t MinShadowCascadesCount = 1;
	static const uint32_t MaxShadowCascadesCount = 4;
	static const uint32_t ShadowCascadeMinSize = 2048;
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;

protected:
	std::vector<glm::mat4> CalculateShadowViewProjectionMatrices(std::shared_ptr<DirectionalLightComponent> component) const;
};