#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Components/CameraComponent.h"
#include "Core/Rendering/Textures/CubeTexture.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(PointLightShadowPassParameters, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(CubeTexture, ShadowMap, Depth, "PointLightPass.ShadowMap")
	
	ED_RENDER_PASS_GRAPH_PARAMETER(std::vector<std::shared_ptr<StaticMeshComponent>>, Meshes, "Scene.StaticMesh")
	ED_RENDER_PASS_GRAPH_OBJECT_PTR_PARAMETER(PointLightComponent, Light, "PointLightPass.Light")
	ED_RENDER_PASS_GRAPH_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(PointLightShadowPassShaderParameters)

	ED_SHADER_PARAMETER(Float, float, FarPlane)
	ED_SHADER_PARAMETER_ARRAY(Mat4, glm::mat4, ViewProjection, 6)
	ED_SHADER_PARAMETER(Float3, glm::vec3, ViewPosition)

	ED_SHADER_PARAMETER(Mat4, glm::mat4, ModelMatrix)

ED_END_SHADER_PARAMETERS_DECLARATION()

class PointLightShadowPass : public RenderPass<PointLightShadowPassParameters, PointLightShadowPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;

protected:
	bool IsActiveLightVisible();
};