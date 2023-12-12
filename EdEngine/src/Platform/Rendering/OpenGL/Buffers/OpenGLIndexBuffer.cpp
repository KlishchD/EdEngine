#include "OpenGLIndexBuffer.h"
#include "Core/Ed.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"

OpenGLIndexBuffer::OpenGLIndexBuffer()
{
	glCreateBuffers(1, &m_Id);
}

void OpenGLIndexBuffer::SetData(void* data, BufferUsage usage)
{
	glNamedBufferData(m_Id, m_Size, data, OpenGLTypes::ConvertBufferUsage(usage));
}

void OpenGLIndexBuffer::SetData(void* data, int32_t size, BufferUsage usage)
{
	m_Size = size;
	glNamedBufferData(m_Id, m_Size, data, OpenGLTypes::ConvertBufferUsage(usage));
}

void OpenGLIndexBuffer::SetSubdata(uint32_t offset, uint32_t size, void* data)
{
	glNamedBufferSubData(m_Id, offset, size, data);
}

uint32_t OpenGLIndexBuffer::GetCount()
{
	return m_Size / sizeof(uint32_t);
}

uint32_t OpenGLIndexBuffer::GetID() const
{
	return m_Id;
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &m_Id);
}
