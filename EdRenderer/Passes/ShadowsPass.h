#pragma once

class RenderGraph;
class RenderPass;

class ShadowsPass : public RenderPass
{
public:
	ShadowsPass();

	virtual void Initialize(RenderGraph* graph) override;

	virtual u32 GetCustomShaderParametersStructSize() override;
    virtual void GatherCustomShaderParameters(void* memory) override;

	virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
	bool AppendViewport(u32 row, u32 column, glm::vec4* viewports, u32& viewport);

	void Submit(CommandList* list, u32 modelIndex, u32 shadowOffset, u32 shadowCount, u32 indices, glm::vec4* viewports, u32& viewport);

protected:
	RenderTarget* m_LocalShadowMaps;

	bool m_HasShadowCastingLights;

	PipelineStateObject* m_LoacalShadowsPSO;
};