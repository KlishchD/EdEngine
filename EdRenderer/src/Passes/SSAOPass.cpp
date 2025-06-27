#include "EdRendererPrivate.h"
#include "SSAOPass.h"
#include "Helpers/MathHelper.h"

// TODO: Add SSAO+TA :)
// https://bartwronski.com/2014/04/27/temporal-supersampling-pt-2-ssao-demonstration/
#define SSAO_TILE_SIZE 32
#define SSAO_MAX_SAMPLES_COUNT 12
#define SSAO_NOISE_TEXTURE_DIMENSIONS 128
#define SSAO_NOISE_TEXTURE_SIZE SSAO_NOISE_TEXTURE_DIMENSIONS * SSAO_NOISE_TEXTURE_DIMENSIONS

SSAOPass::SSAOPass() : RenderPass("SSAOPass")
{
}

void SSAOPass::Initialize(RenderGraph* graph)
{
    RenderPass::Initialize(graph);

    m_Depth = graph->GetRenderTarget("GBuffer.Depth");
    m_Normal = graph->GetRenderTarget("GBuffer.Normal");

    TemporaryArray<glm::vec2> noise(SSAO_NOISE_TEXTURE_SIZE);

    for (u32 i = 0; i < SSAO_NOISE_TEXTURE_SIZE; ++i)
    {
        noise.Add(2.0f * MathHelper::GenerateRandomNormalizedVec2() - 1.0f);
    }

    m_Context->UploadTextureDeferred(reinterpret_cast<u8*>(&noise[0]), SSAO_NOISE_TEXTURE_DIMENSIONS, SSAO_NOISE_TEXTURE_DIMENSIONS, 1, PixelFormat::RG16F, m_Noise, ResourceState::ShaderRead, "SSAONoise");

    m_CurrentSSAO = graph->CreateUAVTarget("SSAO1", PixelFormat::RGBA32F, RenderTargetSizePolicy::Downscale2);
    m_HistorySSAO = graph->CreateUAVTarget("SSAO2", PixelFormat::RGBA32F, RenderTargetSizePolicy::Downscale2);

    std::string tileSizeStr = std::to_string(SSAO_TILE_SIZE);
    std::string maxSampleCountStr = std::to_string(SSAO_MAX_SAMPLES_COUNT);
    std::string noiseTextureDimensionsStr = std::to_string(SSAO_NOISE_TEXTURE_DIMENSIONS);

    {
        ShaderProgram* program = m_Context->CreateShaderProgram(ContentPath("Shaders\\hlsl\\SSAOBase.h"), ST_Compute);
        program->AddDefine(Strings::Concat(32, "SSAO_TILE_SIZE=", tileSizeStr.c_str()));
        program->AddDefine(Strings::Concat(32, "SSAO_MAX_SAMPLES_COUNT=", maxSampleCountStr.c_str()));
        program->AddDefine(Strings::Concat(64, "noiseTextureDimensionsStr=", noiseTextureDimensionsStr.c_str()));
        program->AddDefine("SSAO_BLUR_FILTER_HALF_SIZE=2");

        m_BasePSO = m_Context->CreatePipelineStateObject("SSAOBase", m_Renderer->GetRootSignature(), program);
    }

    {
        ShaderProgram* program = m_Context->CreateShaderProgram(ContentPath("Shaders\\hlsl\\SSAOBlur.h"), ST_Compute);
        program->AddDefine(Strings::Concat(32, "SSAO_TILE_SIZE=", tileSizeStr.c_str()));
        program->AddDefine(Strings::Concat(32, "SSAO_MAX_SAMPLES_COUNT=", maxSampleCountStr.c_str()));
        program->AddDefine(Strings::Concat(32, "noiseTextureDimensionsStr=", noiseTextureDimensionsStr.c_str()));
        program->AddDefine("SSAO_BLUR_FILTER_HALF_SIZE=2");

        m_BlurPSO = m_Context->CreatePipelineStateObject("SSAOBase", m_Renderer->GetRootSignature(), program);
    }
}

u32 SSAOPass::GetCustomShaderParametersStructSize()
{
    return 256;
}

void SSAOPass::GatherCustomShaderParameters(void* memory)
{
    DescriptorHeap* heap = m_Context->GetSRVHeap();

    memory = PutValue(memory, heap->GetDescriptorIndex(m_Depth->GetSRV()));
    memory = PutValue(memory, heap->GetDescriptorIndex(m_Normal->GetSRV()));
    memory = PutValue(memory, heap->GetDescriptorIndex(m_Noise));

    memory = PutValue(memory, NoiseScale);
    memory = PutValue(memory, Radius);
    memory = PutValue(memory, Bias);

    // Curious to see how will it look with sample being regenerated every frame.
    // Probably will see no improvement but curious to test.
    Array<glm::vec3> samples = MathHelper::GenerateHalfSphereSamples(SSAO_MAX_SAMPLES_COUNT, true);

    memory = PutValue(memory, SSAO_MAX_SAMPLES_COUNT);

    for (u32 index = 0; index < SSAO_MAX_SAMPLES_COUNT; ++index)
    {
        memory = PutValue(memory, &samples[0], sizeof(glm::vec3) * SSAO_MAX_SAMPLES_COUNT);
        memory = PutValue(memory, 0.0f); // Paddings (
    }
}

void SSAOPass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
    m_Context->GetGraphicsCommandQueue()->CPUWait();

    list->SetComputeConstantBufferView(1, buffer, offset);
  
    list->SetComputeRootDescriptorTable(8, m_CurrentSSAO->GetView().GPUHandle);
    list->SetComputeRootDescriptorTable(9, m_HistorySSAO->GetView().GPUHandle);

    TemporaryArray<Resource*> transitions;
    transitions.Add(m_CurrentSSAO->GetView().Viewed);
    transitions.Add(m_HistorySSAO->GetView().Viewed);
    list->Transition(transitions, ResourceState::UnorderedAccess);

    Window* window = m_Context->GetWindow();
    u32 width = window->GetWidth();
    u32 height = window->GetHeight();
    
    list->SetPipelineState(m_BasePSO);
    list->Dispatch(Tile(width, SSAO_TILE_SIZE), Tile(height, SSAO_TILE_SIZE), 1);

    list->SetPipelineState(m_BlurPSO);
    list->Dispatch(Tile(width, SSAO_TILE_SIZE), Tile(height, SSAO_TILE_SIZE), 1);

    list->Transition(transitions, ResourceState::Common);
}
