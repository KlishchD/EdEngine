#pragma once

class RenderGraph;
class RenderPass;

class BloomPass : public RenderPass
{
    static constexpr i32 BloomMipsCount = 10;
public:
    BloomPass();

    virtual void Initialize(RenderGraph* graph) override;

    virtual void GetWaitSignals(ccstr8** names, u32& count);

    virtual CommandListType GetCommandQueueType() override { return CommandListType::Compute; }

    virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
    RenderTarget* m_Lighting;
    UAVTarget* m_Target;

    PipelineStateObject* m_DownscalePSO[BloomMipsCount];
    PipelineStateObject* m_UpscalePSO[BloomMipsCount - 1];
};
