#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/SpotLightComponent.h"
#include "Core/Components/CameraComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(SpotLightShadowPass, Base)

	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, ShadowMap, Depth, "SpotLightPass.ShadowMap")
	
	ED_RENDER_PASS_PARAMETER(std::vector<std::shared_ptr<StaticMeshComponent>>, Meshes, "Scene.StaticMesh", Read)

	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(SpotLightComponent, Light,  "SpotLightPass.Light", Read)
	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent,    Camera, "Camera",              Read)

	ED_RENDER_PASS_DECLARE_PARAMETER(glm::mat4, ShadowProjectionViewMatrix, "SpotLightPass.ShadowProjectionViewMatrix")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(SpotLightShadowPass)

	ED_SHADER_PARAMETER(Mat4, glm::mat4, ModelMatrix)

ED_END_SHADER_PARAMETERS_DECLARATION()

class SpotLightShadowPass : public RenderPass<SpotLightShadowPassParameters, SpotLightShadowPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;
};