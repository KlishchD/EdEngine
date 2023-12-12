#include "OpenGLVertexBuffer.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"
#include "Core/Ed.h"

OpenGLVertexBuffer::OpenGLVertexBuffer()
{
	glCreateBuffers(1, &m_Id);
}

void OpenGLVertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	m_Layout = layout;
	m_VertexSize = 0;
	for (const VertexBufferLayoutElement& element : layout.GetElements())
	{
		m_VertexSize += OpenGLTypes::ConvertShaderDataTypeSize(element.Type);
	}
}

void OpenGLVertexBuffer::SetData(void* data, BufferUsage usage)
{
	glNamedBufferData(m_Id, m_Size, data, OpenGLTypes::ConvertBufferUsage(usage));
}

void OpenGLVertexBuffer::SetData(void* data, int32_t size, BufferUsage usage)
{
	m_Size = size;
	glNamedBufferData(m_Id, size, data, OpenGLTypes::ConvertBufferUsage(usage));
}

void OpenGLVertexBuffer::SetSubdata(uint32_t offset, uint32_t size, void* data)
{
	glNamedBufferSubData(m_Id, offset, size, data);
}

uint32_t OpenGLVertexBuffer::GetCount() const
{
	return m_Size / m_VertexSize;
}

uint32_t OpenGLVertexBuffer::GetID() const
{
	return m_Id;
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_Id);
}
