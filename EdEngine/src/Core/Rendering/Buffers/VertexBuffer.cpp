#include "VertexBuffer.h"

void VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	m_Layout = layout;
}

const VertexBufferLayout& VertexBuffer::GetLayout() const
{
	return m_Layout;
}
