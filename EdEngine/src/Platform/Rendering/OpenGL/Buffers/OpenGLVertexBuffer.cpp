#include "OpenGLVertexBuffer.h"
#include "Platform/Rendering/OpenGL/EdOpenGLRendering.h"

OpenGLVertexBuffer::OpenGLVertexBuffer(const std::string& name) : VertexBuffer(name)
{
  glCreateBuffers(1, &m_Id);
  glObjectLabel(GL_BUFFER, m_Id, m_Name.size(), m_Name.c_str());
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
  ED_ASSERT(m_Id, "This buffer was not yet initialized");
  ED_ASSERT(m_Size, "Size was not yet provided for this buffer");
  glNamedBufferData(m_Id, m_Size, data, OpenGLTypes::ConvertBufferUsage(usage));
}

void OpenGLVertexBuffer::SetData(void* data, int32_t size, BufferUsage usage)
{
  ED_ASSERT(m_Id, "This buffer was not yet initialized");

  m_Size = size;
  glNamedBufferData(m_Id, size, data, OpenGLTypes::ConvertBufferUsage(usage));
}

void OpenGLVertexBuffer::SetSubdata(uint32_t offset, uint32_t size, void* data)
{
  ED_ASSERT(m_Id, "This buffer was not yet initialized");
  ED_ASSERT(offset + size <= m_Size, "Attemted to put data outside of the buffer");
  glNamedBufferSubData(m_Id, offset, size, data);
}

uint32_t OpenGLVertexBuffer::GetCount() const
{
  return m_Size / m_VertexSize;
}

void* OpenGLVertexBuffer::GetNativeResource() const
{
  return reinterpret_cast<void*>(m_Id);
}

void OpenGLVertexBuffer::SetNativeResource(void* resource)
{
  m_Id = reinterpret_cast<uint32_t>(resource);
  glObjectLabel(GL_BUFFER, m_Id, m_Name.size(), m_Name.c_str());
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
  glDeleteBuffers(1, &m_Id);
}
