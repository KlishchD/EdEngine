#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/CameraComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(IconsPassParameters, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Color, "Resolution.Color")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Depth, "GBuffer.Depth")

	ED_RENDER_PASS_GRAPH_OBJECT_PTR_PARAMETER(CameraComponent, Camera, "Camera")

	ED_RENDER_PASS_GRAPH_PARAMETER(std::vector<std::shared_ptr<Component>>, Components, "Scene.Component")

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(IconsPassShaderParameters)

ED_END_SHADER_PARAMETERS_DECLARATION()

class IconsPass : public RenderPass<IconsPassParameters, IconsPassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Update(float deltaSeconds) override;

protected:
	std::shared_ptr<Texture2D> m_LightIcon;
};