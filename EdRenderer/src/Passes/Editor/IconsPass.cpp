#include "EdRendererPrivate.h"
#include "IconsPass.h"

// #include "Core/Engine.h"
// #include "Core/Assets/AssetManager.h"
// #include "Core/Components/LightComponent.h"
// 
// void IconsPass::Initialize(std::shared_ptr<RenderGraph> graph)
// {
// 	RenderPass<IconsPassParameters, IconsPassShaderParameters>::Initialize(graph);
// 
// 	// TODO: Think of the way to reslove this thing :)
// 	m_Parameters.DrawFramebuffer->AddAttachment(m_Parameters.Depth.Get());
// 
// 	m_Parameters.Name = "Editor icons";
// 	m_Parameters.Shader = RenderingHelper::CreateShader("Editor\\shaders\\IconShader.glsl");
// 
// 	m_LightIcon = RenderingHelper::ImportBaseColorTexture("Editor\\icons\\light-bulb.png");
// }
// 
// void IconsPass::Execute()
// {
// 	RenderPass<IconsPassParameters, IconsPassShaderParameters>::Execute();
// 
// 	glm::u32vec2 size = m_Renderer->GetViewportSize();
// 	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
// 
// 	Camera& camera = m_Parameters.Camera->GetCamera();
// 
// 	m_Renderer->SetCamera(camera);
// 
// 	for (const std::shared_ptr<Component>& component : m_Parameters.Components.Get())
// 	{
// 		if (std::shared_ptr<LightComponent> light = std::dynamic_pointer_cast<LightComponent>(component))
// 		{
// 			glm::mat4 view = glm::lookAt(light->GetRelativeTransform().GetTranslation(), camera.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
// 			m_Renderer->SubmitIcon(m_LightIcon, glm::scale(glm::inverse(view), light->GetRelativeTransform().GetScale()));
// 		}
// 	}
// }
// 

IconPass::IconPass() : RenderPass("IconPass")
{

}

u32 IconPass::GetCustomShaderParametersStructSize()
{
    const RenderScene& scene = m_Renderer->GetScene();
    return sizeof(IconParameters) * scene.IconElements.GetSize();
}

void IconPass::GatherCustomShaderParameters(void* memory)
{
    const RenderScene& scene = m_Renderer->GetScene();
    for (const IconElement& element : scene.IconElements)
    {
        IconParameters& parmeters = *reinterpret_cast<IconParameters*>(memory);

        parmeters.Tint = element.Tint;
        parmeters.IconIndex = m_Context->GetSRVHeap()->GetDescriptorIndex(element.IconView);
        parmeters.Location = element.Location;
        parmeters.UniformScale = element.UniformScale;

        memory = OffsetBuffer<IconParameters>(memory, 1);
    }
}

void IconPass::Initialize(RenderGraph* graph)
{
    RenderPass::Initialize(graph);

//    GraphicsPipelineStateObjectBuilder builder;
//    builder.SetRootSignature(m_Renderer->GetRootSignature())
//            .SetDepthStencil(true, false)
//            .AddInputElement("Position", 0, PixelFormat::RG32F)
//            .A
}

void IconPass::Execute(CommandList* list, Resource* buffer, u64 offset)
{

}

