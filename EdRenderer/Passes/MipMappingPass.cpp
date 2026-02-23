#include "EdRendererPrivate.h"
#include "MipMappingPass.h"

#define TILE_SIZE 32

MipMappingPass::MipMappingPass() : RenderPass("MipMapping")
{

}

void MipMappingPass::Initialize(RenderGraph* graph)
{
    RenderPass::Initialize(graph);

    m_TemporaryStorage = graph->CreateUAVTarget("MipTemporaryStorage", PixelFormat::RGBA8F, 2048, 2048, 4);

    cstr8 tiles = Strings::Concat(32, "TILE_SIZE=", std::to_string(TILE_SIZE).c_str());

    for (u32 mode = 0; mode < 3; ++mode)
    {
        std::string modeStr = std::to_string(mode);
        for (u32 mips = 0; mips < MaxMipsCount; ++mips)
        {
            std::string mipStr = std::to_string(mips);

            cstr8 name = Strings::Concat(32, "MipMapping_", modeStr.c_str(), "_", mipStr.c_str());

            ComputePipelineStateObjectBuilder builder;
            builder.SetRootSignature(m_Renderer->GetRootSignature())
              .SetShader("MipMapping.h")
              .AddShaderDefine("DOWSCALING_MODE", mode)
              .AddShaderDefine("MIP_COUNT", mips);
            m_AlbedoPSO[mode][mips] = m_Context->CreatePipelineStateObject(name, builder);
        }
    }
}

void MipMappingPass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
    Array<MipMappingRequest>& requests = m_Renderer->GetMipMappingRequests();

    Resource* storage = m_TemporaryStorage->GetResource();

    list->SetComputeRootDescriptorTable(11, m_TemporaryStorage->GetView(0).GPUHandle);

    u32 lastMipCount = -1;
    for (MipMappingRequest& request : requests)
    {
        u32 mode = static_cast<u32>(request.Mode);
        u32 mips = request.Mips;

        if (lastMipCount != mips)
        {
            list->SetPipelineState(m_AlbedoPSO[mode][mips - 1]);
        }


        list->SetComputeRootDescriptorTable(4, request.View.GPUHandle);
        list->Transition(storage, ResourceState::UnorderedAccess);

        Resource* resource = request.View.Viewed;

        u32 width = resource->GetWidth();
        u32 height = resource->GetHeight();

        list->Dispatch(Tile(width, TILE_SIZE), Tile(height, TILE_SIZE), 1);

        list->Transition(storage, ResourceState::CopySource);
        list->Transition(resource, ResourceState::CopyDestination);

        for (u32 i = 1; i < mips; ++i)
        {
            width >>= 1;
            height >>= 1;

            list->CopyTextureRegion(storage, resource, i - 1, i, glm::u32vec2(0), glm::u32vec2(0), glm::vec2(width, height));
        }

        list->Transition(resource, ResourceState::Common);

        m_Context->RequsetDefferedTransition(request.View, ResourceState::ShaderRead);
    }

    requests.Clear();
}
