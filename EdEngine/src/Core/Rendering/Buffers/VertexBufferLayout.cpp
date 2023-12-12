#include "VertexBufferLayout.h"


VertexBufferLayoutElement::VertexBufferLayoutElement(const char* name, ShaderDataType type, bool normalized): Name(name), Type(type), Normalized(normalized)
{
	
}

VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferLayoutElement>& elements): m_Elements(elements)
{
}