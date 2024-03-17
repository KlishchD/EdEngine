#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/DirectionalLightComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(DirectionalLightMultiPassParameters, Multi)

	ED_RENDER_PASS_DECLARE_GRAPH_OBJECT_PTR_PRAMETER(DirectionalLightComponent, Light, "DirectionalLightPass.Light")
	ED_RENDER_PASS_GRAPH_PARAMETER(std::vector<std::shared_ptr<DirectionalLightComponent>>, Lights, "Scene.DirectionalLight")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

class DirectionalLightMultiPass : public MultiPassRenderPass<DirectionalLightMultiPassParameters, ShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;
protected:
	virtual void CreatePasses();
};