#pragma once

class visualization_pass : public RenderPass
{
public:
  visualization_pass();

  virtual void Initialize(RenderGraph* graph) override;
  virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
  RenderTarget* backbuffer;
  PipelineStateObject* pso;
};
