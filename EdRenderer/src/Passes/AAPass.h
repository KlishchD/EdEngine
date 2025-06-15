#pragma once

// #include "Passes/RenderPass.h"
// 
// ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(TAAPass, Base)
// 
// 	ED_RENDER_PASS_DECLARE_RENDER_TARGET(Texture2D, Output, Color16, "TAA.Output")
// 
// 	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, SceneBase,     "LightBuffer.Combined", Read)
// 	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, SceneDepth,    "GBuffer.Depth",        Read)
// 	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, SceneVelocity, "GBuffer.Velocity",     Read)
// 
// ED_END_RENDER_PASS_PARAMETERS_DECLARATION()
// 
// ED_BEGIN_SHADER_PARAMETERS_DECLARATION(TAAPass)
// 
// 	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, PreviousColor)
// 	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, CurrentColor)
// 	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, CurrentDepth)
// 	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Velocity)
// 	
// 	ED_SHADER_PARAMETER(Float2, glm::vec2, PixelSize)
// 	ED_SHADER_PARAMETER(Float2, glm::vec2, ScreenSize)
// 
// 	ED_SHADER_PARAMETER(Float, f32, Gamma)
// 
// ED_END_SHADER_PARAMETERS_DECLARATION()
// 
// class TAAPass : public RenderPass<TAAPassParameters, TAAPassShaderParameters>
// {
// public:
// 	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
// 	virtual void PreExecute() override;
// 	virtual void Execute() override;
// 
// 	void SetGamma(f32 gamma);
// 	f32 GetGamma() const;
// 
// protected:
// 	std::shared_ptr<Texture2D> m_HistoryBuffer;
// };

class RenderGraph;
class RenderPass;

class AAPass : public RenderPass
{
public:
    AAPass();

    virtual void Initialize(RenderGraph* graph) override;

    virtual void GetWaitSignals(ccstr8** names, u32& count);

    virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
    RenderTarget* m_HistoryA;
    RenderTarget* m_HistoryB;

    PipelineStateObject* m_PSO;
};
