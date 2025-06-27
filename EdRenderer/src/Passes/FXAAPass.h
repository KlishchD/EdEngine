// #pragma once
// 
// #include "Passes/RenderPass.h"
// 
// ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(FXAAPass, Compute)
// 
// 	ED_RENDER_PASS_DECLARE_RESOURCE(Texture2D, Output, "FXAA.Output")
// 
// 	ED_RENDER_PASS_RESOURCE_REFERENCE(Texture2D, LightCombined, "LightBuffer.Combined", Read)
// 
// ED_END_SHADER_PARAMETERS_DECLARATION()
// 
// ED_BEGIN_SHADER_PARAMETERS_DECLARATION(FXAAPass)
// 	
// 	ED_SHADER_PARAMETER_PTR(Texture, Texture2D, Input)
// 	ED_SHADER_PARAMETER_PTR(Image, Texture2D, Output)
// 
// 	ED_SHADER_PARAMETER(Float, f32, ContrastThreshold)
// 	ED_SHADER_PARAMETER(Float, f32, RelativeThreshold)
// 	ED_SHADER_PARAMETER(Float, f32, SubpixelBlending)
// 
// 	ED_SHADER_PARAMETER(Float2, glm::vec2, PixelSize)
// 
// ED_END_SHADER_PARAMETERS_DECLARATION()
// 
// class FXAAPass : public RenderPass<FXAAPassParameters, FXAAPassShaderParameters>
// {
// public:
// 	virtual void Initialize(std::shared_ptr<RenderGraph> graph);
// 	virtual void Execute();
// 
// 	void SetContrastThreshold(f32 threshold);
// 	f32 GetContrastThreshold() const;
// 
// 	void SetRelativeThreshold(f32 threshold);
// 	f32 GetRelativeThreshold() const;
// 
// 	void SetSubpixelBlending(f32 scale);
// 	f32 GetSubpixelBlending() const;
// };