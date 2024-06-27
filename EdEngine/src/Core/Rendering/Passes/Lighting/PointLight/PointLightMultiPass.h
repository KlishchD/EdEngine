#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Components/CameraComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(PointLightMultiPass, Multi)

	ED_RENDER_PASS_DECLARE_OBJECT_PTR_PRAMETER(PointLightComponent, Light, "PointLightPass.Light")

	ED_RENDER_PASS_DECLARE_PARAMETER(std::vector<glm::vec3>, LightMeshVertices, "PointLightPass.LightMeshVertices")
	ED_RENDER_PASS_DECLARE_PARAMETER(std::vector<int32_t>,   LightMeshIndices,  "PointLightPass.LightMeshIndices")

	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera", Read)
	
	ED_RENDER_PASS_PARAMETER(std::vector<std::shared_ptr<PointLightComponent>>, Lights, "Scene.PointLight", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

class PointLightMultiPass : public MultiPassRenderPass<PointLightMultiPassParameters>
{
	static const uint32_t PointLightMeshSectorsCount = 30;
	static const uint32_t PointLightMeshStackCount = 30;
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;
protected:
	virtual void CreatePasses();

	bool IsActiveLightVisible();
};