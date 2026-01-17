#include "EdRenderApiPrivate.h"
#include "PipelineStateObject.h"

GraphicsPipelineStateObjectBuilder& GraphicsPipelineStateObjectBuilder::SetShaderProgram(const ShaderPath& vertex, const ShaderPath& pixel)
{
    m_Program = RenderingContext::Get().CreateShaderProgram(vertex, pixel);
    return *this;
}

GraphicsPipelineStateObjectBuilder& GraphicsPipelineStateObjectBuilder::SetShaderProgram(const ShaderPath& path, ShaderType types)
{
    m_Program = RenderingContext::Get().CreateShaderProgram(path, types);
    return *this;
}
