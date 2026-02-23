#include "EdRendererPrivate.h"
#include "visualization.h"
#include "visualization_pass.h"

visualization_pass::visualization_pass() : RenderPass("visualization")
{

}

void visualization_pass::Initialize(RenderGraph* graph)
{
  RenderPass::Initialize(graph);

  // Scene - is kind of a backbuffer to be used in ImGui.
  // Will need to rework this for Standalone build solution.
  backbuffer = graph->GetRenderTarget("Scene");

  {
    ComputePipelineStateObjectBuilder builder;
    builder.SetRootSignature(m_Renderer->GetRootSignature())
      .SetShader("visualization.h");

    pso = RenderingContext::Get().CreatePipelineStateObject("visualization", builder);
  }
}

void visualization_pass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
  const auto& parameters = visualization::parameters;

  ResourceView sources[3]{};

  switch (parameters.mode)
  {
  case visualization::gbuffer_albedo:
  {
    sources[0] = m_Graph->GetRenderTarget("GBuffer.Albedo")->GetSRV();
    break;
  }
  case visualization::gbuffer_normals:
  {
    sources[0] = m_Graph->GetRenderTarget("GBuffer.Normal")->GetSRV();
    break;
  }
  case visualization::custom:
  {
    if (!parameters.custom_target)
    {
      ED_LOG(visualization, err, "Custom visualization mode requires target.");
      visualization::parameters.mode = visualization::none;
      return;
    }

    if (parameters.custom_target.Type != DescriptorHeapType::SRV)
    {
      ED_LOG(visualization, err, "Custom visualization mode requires SRV view for target.");
      visualization::parameters.mode = visualization::none;
      return;
    }

    sources[0] = parameters.custom_target;
    break;
  }
  default: return;
  }

  list->Transition(sources, ResourceState::NonPixelShaderResource);
  list->Transition(backbuffer->GetUAV(), ResourceState::UnorderedAccess);
  list->SetComputeRootDescriptorTable(8, backbuffer->GetUAV().GPUHandle);

  list->SetPipelineState(pso);

  const auto& heap = m_Context->GetSRVHeap();

  u32 constants[] = {
    static_cast<u32>(parameters.mode),
    sources[0] ? heap->GetDescriptorIndex(sources[0]) : 0,
    sources[1] ? heap->GetDescriptorIndex(sources[1]) : 0,
    sources[2] ? heap->GetDescriptorIndex(sources[0]) : 0,
  };

  list->SetComputeRootConstant(12, std::size(constants), 0, constants);

  auto size = m_Context->GetWindow()->GetSize();
  list->Dispatch(Tile(size.x, 16), Tile(size.y, 16), 1);

  list->UAVBarrier(backbuffer->GetUAV().Viewed);

  list->Transition(backbuffer->GetUAV(), ResourceState::ShaderRead);
  list->Transition(sources, ResourceState::ShaderRead);
}