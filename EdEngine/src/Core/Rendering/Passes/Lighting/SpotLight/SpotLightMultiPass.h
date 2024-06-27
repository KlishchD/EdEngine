#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/SpotLightComponent.h"
#include "Core/Components/CameraComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(SpotLightMultiPass, Multi)

	ED_RENDER_PASS_DECLARE_OBJECT_PTR_PRAMETER(SpotLightComponent, Light, "SpotLightPass.Light")
	
	ED_RENDER_PASS_DECLARE_PARAMETER(std::vector<glm::vec3>, LightMeshVertices, "SpotLightPass.LightMeshVertices")
	ED_RENDER_PASS_DECLARE_PARAMETER(std::vector<int32_t>,   LightMeshIndices,  "SpotLightPass.LightMeshIndices")

	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera", Read)

	ED_RENDER_PASS_PARAMETER(std::vector<std::shared_ptr<SpotLightComponent>>, Lights, "Scene.SpotLight", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

class SpotLightMultiPass : public MultiPassRenderPass<SpotLightMultiPassParameters>
{
	static const int32_t SpotLightMeshSectorsCount = 50;
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;
protected:
	virtual void CreatePasses();

	bool IsActiveLightVisible();
};