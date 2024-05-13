#include "IconsPass.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Components/LightComponent.h"

void IconsPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<IconsPassParameters, IconsPassShaderParameters>::Initialize(graph);

	// TODO: Think of the way to reslove this thing :)
	m_Parameters.DrawFramebuffer->AddAttachment(m_Parameters.Depth.Get());

	m_Parameters.Name = "Editor icons";
	m_Parameters.Shader = RenderingHelper::CreateShader("Editor\\shaders\\IconShader.glsl");

	m_LightIcon = RenderingHelper::ImportBaseColorTexture("Editor\\icons\\light-bulb.png");
}

void IconsPass::Execute()
{
	RenderPass<IconsPassParameters, IconsPassShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);

	Camera& camera = m_Parameters.Camera->GetCamera();

	m_Renderer->SetCamera(camera);

	for (const std::shared_ptr<Component>& component : m_Parameters.Components.Get())
	{
		if (std::shared_ptr<LightComponent> light = std::dynamic_pointer_cast<LightComponent>(component))
		{
			glm::mat4 view = glm::lookAt(light->GetRelativeTransform().GetTranslation(), camera.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
			m_Renderer->SubmitIcon(m_LightIcon, glm::scale(glm::inverse(view), light->GetRelativeTransform().GetScale()));
		}
	}
}

