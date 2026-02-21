#include "EdRendererPrivate.h"
#include "DepthPrepass.h"

DepthPrepass::DepthPrepass() : RenderPass("DepthPrepass")
{

}

void DepthPrepass::Initialize(RenderGraph* graph)
{
    RenderPass::Initialize(graph);

    m_Depth = graph->CreateRenderTarget("GBuffer.Depth", PixelFormat::Depth, RenderTargetSizePolicy::Full, false);

    {
        GraphicsPipelineStateObjectBuilder builder;
        builder.SetRootSignature(m_Renderer->GetRootSignature())
            .SetShader("DepthPrepass.h")
            .SetDepthStencil(true, false)
            .SetDepthFormat(PixelFormat::Depth)
            .SetDepthFunction(ComparisonFunction::Greater)
            .AddInputElement("Position", 0, PixelFormat::RGB32F)
            .AddInputElement("Color", 0, PixelFormat::RGBA32F)
            .AddInputElement("UVs", 0, PixelFormat::RGB32F)
            .AddInputElement("Normals", 0, PixelFormat::RGB32F)
            .AddInputElement("Tangents", 0, PixelFormat::RGB32F)
            .AddInputElement("Bitangent", 0, PixelFormat::RGB32F);

        m_PSO = m_Context->CreatePipelineStateObject("DepthPrepass", builder);
    }
}

void DepthPrepass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
    list->Transition(m_Depth->GetView(), ResourceState::DepthWrite);
    list->ClearDepthTarget(m_Depth->GetView(), 0.0f);

    const RenderScene& scene = m_Renderer->GetScene();
    const u32 meshCount = scene.StaticMeshes.GetSize();

    if (meshCount == 0)
    {
        return;
    }

    list->Transition(m_Context->GetGeometryVertexPool(), ResourceState::VertexAndConstantBuffer);
    list->Transition(m_Context->GetGeometryIndexPool(), ResourceState::IndexBuffer);

    list->SetRenderTargets(ResourceView(), m_Depth->GetView());
    list->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

    list->SetPipelineState(m_PSO);
    
    Window* window = m_Context->GetWindow();
    list->SetViewport({ 0.0f, 0.0f }, { window->GetWidth(), window->GetHeight() }, { 0.0f, 1.0f });
    list->SetScissor({ 0.0f, 0.0f }, { window->GetWidth(), window->GetHeight() });

    for (u32 index = 0; index < meshCount; ++index)
    {
        const StaticMeshElement& element = scene.StaticMeshes[index];

        glm::mat4 world = element.WorldTransform->GetMatrix();
        list->SetGraphicsRootConstants(12, 16, 0, reinterpret_cast<u32*>(glm::value_ptr(world)));

        list->SetVertexBuffer(element.VertexBufferView, 0, m_PSO->GetVertexStride());
        list->SetIndexBuffer(element.IndexBufferView);

        list->DrawIndexed(0, element.IndexBufferView.Size / sizeof(u32), 0);
    }
}