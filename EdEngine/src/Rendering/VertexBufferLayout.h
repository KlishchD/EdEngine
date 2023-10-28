#pragma once

#include <string>
#include <vector>
#include "Utils/RenderAPIUtils.h"

struct VertexBufferLayoutElement {
    std::string Name;
    uint32_t Count;
    uint32_t Type;
    uint32_t Size;
    bool Normalized;
    uint32_t Offset;
	
    VertexBufferLayoutElement(const char* name, uint32_t count, ShaderDataType type, bool normalized);
    VertexBufferLayoutElement(const char* name, uint32_t count, ShaderDataType type);
    VertexBufferLayoutElement(const std::string& name, uint32_t count, ShaderDataType type);
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() {}
    VertexBufferLayout(const std::initializer_list<VertexBufferLayoutElement>& elements);

    const std::vector<VertexBufferLayoutElement>& GetElements() const { return m_Elements; }

    uint32_t GetStride() const { return m_Stride; }
private:
    std::vector<VertexBufferLayoutElement> m_Elements;
    uint32_t m_Stride;
};