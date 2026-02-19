#include "EdRendererPrivate.h"
#include "BloomPass.h"
#include "Console.h"

#define BLOOM_TILE_SIZE 16

static auto& r_bloom_strength = console::create_f32("r_bloom_strength", 0.85f, 0.0f, 2.0f);

BloomPass::BloomPass() : RenderPass("BloomPass")
{
}

void BloomPass::Initialize(RenderGraph* graph)
{
    RenderPass::Initialize(graph);

    m_Lighting = graph->GetRenderTarget("Lighting");
    m_Target = graph->CreateUAVTarget("Bloom", PixelFormat::R11G11B10F, RenderTargetSizePolicy::Downscale2, BloomMipsCount);

    std::string mipsCountStr = std::to_string(BloomMipsCount);
    std::string tileSizeStr = std::to_string(BLOOM_TILE_SIZE);

    for (i32 mip = 0; mip < BloomMipsCount; ++mip)
    {
        std::string mipStr = std::to_string(mip).c_str();
        ccstr8 name = Strings::Concat(32, "BloomDownscale_", mipStr.c_str());

        ComputePipelineStateObjectBuilder builder;
        builder.SetRootSignature(m_Renderer->GetRootSignature())
          .SetShader("BloomDownscale.h")
          .AddShaderDefine("BLOOM_MIPS_COUNT", BloomMipsCount)
          .AddShaderDefine("BLOOM_MIP_LEVEL", mip)
          .AddShaderDefine("BLOOM_TILE_SIZE", BLOOM_TILE_SIZE);

        m_DownscalePSO[mip] = m_Context->CreatePipelineStateObject(name, builder);

    }

    for (i32 mip = 0; mip < BloomMipsCount - 1; ++mip)
    {
        std::string mipStr = std::to_string(mip).c_str();
        ccstr8 name = Strings::Concat(32, "BloomUpscale_", mipStr.c_str());

        ComputePipelineStateObjectBuilder builder;
        builder.SetRootSignature(m_Renderer->GetRootSignature())
          .SetShader("BloomUpscale.h")
          .AddShaderDefine("BLOOM_MIPS_COUNT", BloomMipsCount)
          .AddShaderDefine("BLOOM_MIP_LEVEL", mip)
          .AddShaderDefine("BLOOM_TILE_SIZE", BLOOM_TILE_SIZE);

        m_UpscalePSO[mip] = m_Context->CreatePipelineStateObject(name, builder);
    }
}

void BloomPass::GetWaitSignals(ccstr8** names, u32& count)
{
    static ccstr8 dependencies[] = { "LigthingPass" };
    (*names) = dependencies; 
    count = std::size(dependencies);
}

void BloomPass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
    list->Transition(m_Target->GetResource(), ResourceState::UnorderedAccess);

    list->SetComputeRootDescriptorTable(4, m_Lighting->GetSRV().GPUHandle);
    list->SetComputeRootDescriptorTable(5, m_Target->GetSRV().GPUHandle);
    list->SetComputeRootDescriptorTable(11, m_Target->GetView().GPUHandle);

    glm::u32vec2 size(m_Context->GetWindow()->GetWidth(), m_Context->GetWindow()->GetHeight());

    for (i32 mip = 0; mip < BloomMipsCount; ++mip)
    {
        list->SetPipelineState(m_DownscalePSO[mip]);
    
        u32 width = std::max<u32>(1, size.x >> (mip + 1));
        u32 height = std::max<u32>(1, size.y >> (mip + 1));
    
        list->Dispatch(Tile(width, BLOOM_TILE_SIZE), Tile(height, BLOOM_TILE_SIZE), 1);

        list->UAVBarrier(m_Target->GetResource());
    }

    for (i32 mip = BloomMipsCount - 2; mip >= 0; --mip)
    {
        list->SetPipelineState(m_UpscalePSO[mip]);

        list->SetComputeRootConstant(12, 1, 0, reinterpret_cast<u32*>(&r_bloom_strength()));
    
        u32 width = std::max<u32>(1, size.x >> (mip + 1));
        u32 height = std::max<u32>(1, size.y >> (mip + 1));
    
        list->Dispatch(Tile(width, BLOOM_TILE_SIZE), Tile(height, BLOOM_TILE_SIZE), 1);

        list->UAVBarrier(m_Target->GetResource());
    }

    list->Transition(m_Target->GetResource(), ResourceState::Common);
}