#pragma once

class RenderGraph;
class RenderPass;

class ResolutionPass : public RenderPass
{
public:
    ResolutionPass();

    virtual void Initialize(RenderGraph* graph) override;

    virtual void GetWaitSignals(ccstr8** names, u32& count) override;

    virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
    RenderTarget* m_Scene;
    RenderTarget* m_Lighting;
    UAVTarget* m_Boom;

    PipelineStateObject* m_PSO;
};
