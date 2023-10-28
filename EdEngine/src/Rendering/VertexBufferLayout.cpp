#include "VertexBufferLayout.h"


VertexBufferLayoutElement::VertexBufferLayoutElement(const char* name, uint32_t count, ShaderDataType type, bool normalized):
    Name(name),
    Count(count * RenderAPIUtils::GetShaderDataTypeOpenGLComponentCount(type)),
    Type(RenderAPIUtils::GetShaderDataTypeOpenGLType(type)),
    Size(RenderAPIUtils::GetShaderDataTypeOpenGLSize(type)),
    Normalized(normalized),
    Offset(0)
{
	
}

VertexBufferLayoutElement::VertexBufferLayoutElement(const char* name, uint32_t count, ShaderDataType type):
    Name(name),
    Count(count * RenderAPIUtils::GetShaderDataTypeOpenGLComponentCount(type)),
    Type(RenderAPIUtils::GetShaderDataTypeOpenGLType(type)),
    Size(RenderAPIUtils::GetShaderDataTypeOpenGLSize(type)),
    Normalized(false),
    Offset(0)
{
	
}

VertexBufferLayoutElement::VertexBufferLayoutElement(const std::string& name, uint32_t count, ShaderDataType type):
    Name(name),
    Count(count * RenderAPIUtils::GetShaderDataTypeOpenGLComponentCount(type)),
    Type(RenderAPIUtils::GetShaderDataTypeOpenGLType(type)),
    Size(RenderAPIUtils::GetShaderDataTypeOpenGLSize(type)),
    Normalized(false),
    Offset(0)
{

}

VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferLayoutElement>& elements): m_Elements(elements)
{
    uint32_t offset = 0;
    for (auto& element : m_Elements) {
        element.Offset = offset;
        offset += element.Size;
    }
    m_Stride = offset;
}