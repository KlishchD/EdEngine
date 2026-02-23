#pragma once

class RenderGraph;
class RenderPass;

class GBufferPass : public RenderPass
{
  struct ObjectParameters
  {
    f32 WorldTransform[16];
    f32 PreviousWorldTransfom[16];
  };
public:
  GBufferPass();

  virtual void Initialize(RenderGraph* graph) override;

  virtual u32 GetCustomShaderParametersStructSize() override;
  virtual void GatherCustomShaderParameters(void* memory) override;

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