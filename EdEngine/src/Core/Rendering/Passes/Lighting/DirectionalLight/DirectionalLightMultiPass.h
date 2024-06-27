#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/DirectionalLightComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(DirectionalLightMultiPass, Multi)

	ED_RENDER_PASS_DECLARE_OBJECT_PTR_PRAMETER(DirectionalLightComponent, Light, "DirectionalLightPass.Light")
	
	ED_RENDER_PASS_PARAMETER(std::vector<std::shared_ptr<DirectionalLightComponent>>, Lights, "Scene.DirectionalLight", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

class DirectionalLightMultiPass : public MultiPassRenderPass<DirectionalLightMultiPassParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;
protected:
	virtual void CreatePasses();
};