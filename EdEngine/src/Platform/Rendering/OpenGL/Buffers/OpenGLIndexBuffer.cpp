#include "OpenGLIndexBuffer.h"
#include "Core/Rendering/EdRendering.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"

OpenGLIndexBuffer::OpenGLIndexBuffer()
{
	glCreateBuffers(1, &m_Id);
}

void OpenGLIndexBuffer::SetData(void* data, BufferUsage usage)
{
	ED_ASSERT(m_Id, "This buffer was not yet initialized");
	ED_ASSERT(m_Size, "Size was not yet provided for this buffer");
	glNamedBufferData(m_Id, m_Size, data, OpenGLTypes::ConvertBufferUsage(usage));
}

void OpenGLIndexBuffer::SetData(void* data, int32_t size, BufferUsage usage)
{
	ED_ASSERT(m_Id, "This buffer was not yet initialized");

	m_Size = size;
	glNamedBufferData(m_Id, m_Size, data, OpenGLTypes::ConvertBufferUsage(usage));
}

void OpenGLIndexBuffer::SetSubdata(uint32_t offset, uint32_t size, void* data)
{
	ED_ASSERT(m_Id, "This buffer was not yet initialized");
	ED_ASSERT(offset + size <= m_Size, "Attemted to put data outside of the buffer");
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
