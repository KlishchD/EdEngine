// #pragma once
// 
// #include "Passes/RenderPass.h"
// #include "Core/Components/PointLightComponent.h"
// #include "Core/Components/CameraComponent.h"
// 
// ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(PointLightMultiPass, Multi)
// 
// 	ED_RENDER_PASS_DECLARE_OBJECT_PTR_PRAMETER(PointLightComponent, Light, "PointLightPass.Light")
// 
// 	ED_RENDER_PASS_DECLARE_PARAMETER(std::vector<glm::vec3>, LightMeshVertices, "PointLightPass.LightMeshVertices")
// 	ED_RENDER_PASS_DECLARE_PARAMETER(std::vector<i32>,   LightMeshIndices,  "PointLightPass.LightMeshIndices")
// 
// 	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera", Read)
// 	
// 	ED_RENDER_PASS_PARAMETER(std::vector<std::shared_ptr<PointLightComponent>>, Lights, "Scene.PointLight", Read)
// 
// ED_END_RENDER_PASS_PARAMETERS_DECLARATION()
// 
// class PointLightMultiPass : public MultiPassRenderPass<PointLightMultiPassParameters>
// {
// 	static const u32 PointLightMeshSectorsCount = 30;
// 	static const u32 PointLightMeshStackCount = 30;
// public:
// 	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
// 	virtual void Execute() override;
// protected:
// 	virtual void CreatePasses();
// 
// 	bool IsActiveLightVisible();
// };