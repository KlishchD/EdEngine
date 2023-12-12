#pragma once

#include <string>
#include <vector>
#include "Core/Rendering/Types.h"

struct VertexBufferLayoutElement {
    std::string Name;
    ShaderDataType Type;
    bool Normalized;
	
    VertexBufferLayoutElement(const char* name, ShaderDataType type, bool normalized = false);
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() {}
    VertexBufferLayout(const std::initializer_list<VertexBufferLayoutElement>& elements);

    const std::vector<VertexBufferLayoutElement>& GetElements() const { return m_Elements; }
private:
    std::vector<VertexBufferLayoutElement> m_Elements;
};