#include "EdRenderApiPrivate.h"
#include "PipelineStateObject.h"

GraphicsPipelineStateObjectBuilder& GraphicsPipelineStateObjectBuilder::SetShaderProgram(const ContentPath& vertex, const ContentPath& pixel)
{
    m_Program = RenderingContext::Get().CreateShaderProgram(vertex, pixel);
    return *this;
}

GraphicsPipelineStateObjectBuilder& GraphicsPipelineStateObjectBuilder::SetShaderProgram(const ContentPath& path, ShaderType types)
{
    m_Program = RenderingContext::Get().CreateShaderProgram(path, types);
    return *this;
}
