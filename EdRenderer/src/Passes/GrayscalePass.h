#pragma once

// #include "Passes/RenderPass.h"
// 
// ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(GrayscalePass, Base)
// 
// 	ED_RENDER_PASS_RENDER_TARGET_REFERENCE(Texture, Color, "Resolution.Color")
// 
// ED_END_RENDER_PASS_PARAMETERS_DECLARATION()
// 
// ED_BEGIN_SHADER_PARAMETERS_DECLARATION(GrayscalePass)
// 
// 	ED_SHADER_PARAMETER_PTR(Texture, Texture, Color)
// 
// ED_END_SHADER_PARAMETERS_DECLARATION()
// 
// class GrayscalePass : public RenderPass<GrayscalePassParameters, GrayscalePassShaderParameters>
// {
// public:
// 	virtual void Initialize(std::shared_ptr<RenderGraph> graph) override;
// 	virtual void Execute() override;
// };
