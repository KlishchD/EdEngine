#include "EdRendererPrivate.h"
#include "BloomPass.h"

#define BLOOM_TILE_SIZE 16

CONSOLE_VARIABLE(r_BloomStrength, f32, 1.0f)

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

        ShaderProgram* program = m_Context->CreateShaderProgram(ContentPath("Shaders\\hlsl\\BloomDownscale.h"), ST_Compute);
        program->AddDefine(Strings::Concat(32, "BLOOM_MIPS_COUNT=", mipsCountStr.c_str()));
        program->AddDefine(Strings::Concat(32, "BLOOM_MIP_LEVEL=", mipStr.c_str()));
        program->AddDefine(Strings::Concat(32, "BLOOM_TILE_SIZE=", tileSizeStr.c_str()));
        m_DownscalePSO[mip] = m_Context->CreatePipelineStateObject(name, m_Renderer->GetRootSignature(), program);

    }

    for (i32 mip = 0; mip < BloomMipsCount - 1; ++mip)
    {
        std::string mipStr = std::to_string(mip).c_str();
        ccstr8 name = Strings::Concat(32, "BloomUpscale_", mipStr.c_str());

        ShaderProgram* program = m_Context->CreateShaderProgram(ContentPath("Shaders\\hlsl\\BloomUpscale.h"), ST_Compute);
        program->AddDefine(Strings::Concat(32, "BLOOM_MIPS_COUNT=", mipsCountStr.c_str()));
        program->AddDefine(Strings::Concat(32, "BLOOM_MIP_LEVEL=", mipStr.c_str()));
        program->AddDefine(Strings::Concat(32, "BLOOM_TILE_SIZE=", tileSizeStr.c_str()));
        m_UpscalePSO[mip] = m_Context->CreatePipelineStateObject(name, m_Renderer->GetRootSignature(), program);
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
    static u32 frame = 0;
    r_BloomStrength(static_cast<f32>(frame % 10));
    ++frame;

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
    
        static f32 BloomUpscaleStrength = 0.85f; // TODO: Move after adding commands
        list->SetComputeRootConstant(12, 1, 0, reinterpret_cast<u32*>(&BloomUpscaleStrength));
    
        u32 width = std::max<u32>(1, size.x >> (mip + 1));
        u32 height = std::max<u32>(1, size.y >> (mip + 1));
    
        list->Dispatch(Tile(width, BLOOM_TILE_SIZE), Tile(height, BLOOM_TILE_SIZE), 1);

        list->UAVBarrier(m_Target->GetResource());
    }

    list->Transition(m_Target->GetResource(), ResourceState::Common);
}