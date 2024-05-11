#pragma once

#include "Core/Rendering/Passes/RenderPass.h"
#include "Core/Components/SpotLightComponent.h"
#include "Core/Components/CameraComponent.h"

ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(SpotLightWireframePass, Base)

	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Diffuse,  "LightBuffer.Diffuse")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Specular, "LightBuffer.Specular")
	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture2D, Combined, "LightBuffer.Combined")

	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(SpotLightComponent, Light,  "SpotLightPass.Light", Read)
	ED_RENDER_PASS_OBJECT_PTR_PARAMETER(CameraComponent,    Camera, "Camera"             , Read)

	ED_RENDER_PASS_RESOURCE_REFERENCE(VertexBuffer, LightMeshVBO, "SpotLightPass.LightMeshVBO", Read)
	ED_RENDER_PASS_RESOURCE_REFERENCE(IndexBuffer,  LightMeshIBO, "SpotLightPass.LightMeshIBO", Read)

ED_END_RENDER_PASS_PARAMETERS_DECLARATION()

ED_BEGIN_SHADER_PARAMETERS_DECLARATION(SpotLightWireframePass)

	ED_SHADER_PARAMETER(Mat4, glm::mat4, ModelMatrix)

ED_END_SHADER_PARAMETERS_DECLARATION()

class SpotLightWireframePass : public RenderPass<SpotLightWireframePassParameters, SpotLightWireframePassShaderParameters>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
	virtual void Execute() override;
};