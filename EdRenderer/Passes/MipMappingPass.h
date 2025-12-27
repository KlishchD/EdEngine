#pragma once

class RenderGraph;
class RenderPass;

class MipMappingPass : public RenderPass
{
public:
	static constexpr u32 MaxMipsCount = 5;
public:
	MipMappingPass();

    virtual CommandListType GetCommandQueueType() override { return CommandListType::Compute; }
	virtual void Initialize(RenderGraph* graph) override;
	virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
	PipelineStateObject* m_AlbedoPSO[3][5];

	UAVTarget* m_TemporaryStorage;
};