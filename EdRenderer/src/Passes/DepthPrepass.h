#pragma once

class RenderGraph;
class RenderPass;

class DepthPrepass : public RenderPass
{
public:
	DepthPrepass();

	virtual void Initialize(RenderGraph* graph) override;

	virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
	RenderTarget* m_Albedo;
	RenderTarget* m_Normal;
	RenderTarget* m_Material;
	RenderTarget* m_Velocity;
	RenderTarget* m_Depth;

	PipelineStateObject* m_PSO;

	i32 m_JitterSequenceSize = 16;
	i32 m_CurrentJitterIndex = 0;
	std::vector<glm::vec2> m_JitterSequence;

	glm::mat4 m_PreviousView;
};