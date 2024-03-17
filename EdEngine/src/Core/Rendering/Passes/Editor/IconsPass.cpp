#include "IconsPass.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Components/LightComponent.h"

void IconsPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<IconsPassParameters, IconsPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "Editor icons";
	m_Parameters.Shader = RenderingHelper::CreateShader("Editor\\shaders\\IconShader.glsl");

	std::shared_ptr<AssetManager> assetManager = Engine::Get().GetManager<AssetManager>();
	m_LightIcon = assetManager->LoadTexture(RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(Files::ContentFolderPath + R"(Editor\icons\light-bulb.png)"));
}

void IconsPass::Update(float deltaSeconds)
{
	RenderPass<IconsPassParameters, IconsPassShaderParameters>::Update(deltaSeconds);

	if (m_Renderer->IsViewportSizeDirty())
	{
		glm::u32vec2 size = m_Renderer->GetViewportSize();
		m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
	}

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

