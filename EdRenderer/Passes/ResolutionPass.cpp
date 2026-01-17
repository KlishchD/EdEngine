#include "EdRendererPrivate.h"
#include "ResolutionPass.h"

ResolutionPass::ResolutionPass() : RenderPass("Resolution")
{

}

void ResolutionPass::Initialize(RenderGraph* graph)
{
    RenderPass::Initialize(graph);

    m_Scene = graph->CreateRenderTarget("Scene", PixelFormat::RGBA8F, RenderTargetSizePolicy::Full);
    m_Lighting = graph->GetRenderTarget("Lighting");
    m_Boom = graph->GetUAVTarget("Bloom");

    {
        GraphicsPipelineStateObjectBuilder builder;
        builder.SetRootSignature(m_Renderer->GetRootSignature())
            .SetShaderProgram("Resolution.h", static_cast<ShaderType>(ST_Vertex | ST_Pixel))
            .AddInputElement("Position", 0, PixelFormat::RG32F)
            .AddRenderTarget().SetFormat(PixelFormat::RGBA8F);

        m_PSO = RenderingContext::Get().CreatePipelineStateObject("Resolution", builder);
    }

}

void ResolutionPass::GetWaitSignals(ccstr8** names, u32& count)
{
    static ccstr8 signals[] = { "BloomPass" };
    (*names) = signals;
    count = std::size(signals);
}

void ResolutionPass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
    list->Transition(m_Scene->GetView(), ResourceState::RenderTarget);
    list->Transition(m_Boom->GetResource(), ResourceState::ShaderRead);

    list->SetRenderTargets(m_Scene->GetView(), ResourceView());
    list->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    
    list->SetPipelineState(m_PSO);

    Window* window = m_Context->GetWindow();
    list->SetViewport({ 0.0f, 0.0f }, { window->GetWidth(), window->GetHeight() }, { 0.0f, 1.0f });
    list->SetScissor({ 0.0f, 0.0f }, { window->GetWidth(), window->GetHeight() });

    list->SetGraphicsRootDescriptorTable(4, m_Lighting->GetSRV().GPUHandle);
    list->SetGraphicsRootDescriptorTable(5, m_Boom->GetSRV().GPUHandle);

    // TODO: Move after adding commands
    f32 Gamma = 2.2f;
    f32 BloomStrength = 0.1f;
    f32 BloomIntensity = 1.0f;
    f32 parameters[] = { Gamma, BloomStrength, BloomIntensity };

    // TODO: Probably will be a good idea to move on compute.
    list->SetGraphicsRootConstants(12, 3, 0, reinterpret_cast<u32*>(parameters));

    m_Renderer->DrawFullscreenQuad(list);

    list->Transition(m_Boom->GetResource(), ResourceState::Common);
    list->Transition(m_Scene->GetView(), ResourceState::ShaderRead);
}
