#include "OpenGLUniformBuffer.h"
#include "Core/Rendering/EdRendering.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"

OpenGLUniformBuffer::OpenGLUniformBuffer()
{
	glGenBuffers(1, &m_Id);
}

void OpenGLUniformBuffer::SetData(void* data, BufferUsage usage)
{
	ED_ASSERT(m_Id, "This buffer was not yet initialized");
	ED_ASSERT(m_Size, "Size was not yet provided for this buffer");

	glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
	glBufferData(GL_UNIFORM_BUFFER, m_Size, data, OpenGLTypes::ConvertBufferUsage(usage));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::SetData(void* data, int32_t size, BufferUsage usage)
{
	ED_ASSERT(m_Id, "This buffer was not yet initialized");

	m_Size = size;

	glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
	glBufferData(GL_UNIFORM_BUFFER, m_Size, data, OpenGLTypes::ConvertBufferUsage(usage));
	glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
}

void OpenGLUniformBuffer::SetSubdata(uint32_t offset, uint32_t size, void* data)
{
	ED_ASSERT(m_Id, "This buffer was not yet initialized");
	ED_ASSERT(offset + size <= m_Size, "Attemted to put data outside of the buffer");

	glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

uint32_t OpenGLUniformBuffer::GetID() const
{
	return m_Id;
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
	glDeleteBuffers(1, &m_Id);
}
