#pragma once

class RenderGraph;
class RenderPass;

class SSAOPass : public RenderPass
{
public:
    SSAOPass();

    virtual void Initialize(RenderGraph* graph) override;

    virtual u32 GetCustomShaderParametersStructSize() override;
    virtual void GatherCustomShaderParameters(void* memory) override;

    virtual CommandListType GetCommandQueueType() override { return CommandListType::Compute; }

    virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
    RenderTarget* m_Depth;
    RenderTarget* m_Normal;
    ResourceView m_Noise;
    UAVTarget* m_CurrentSSAO;
    UAVTarget* m_HistorySSAO;

    PipelineStateObject* m_BasePSO;
    PipelineStateObject* m_BlurPSO;

    f32 NoiseScale = 10.0f;
    f32 Radius = 1.5f;
    f32 Bias = 0.025f;
};
