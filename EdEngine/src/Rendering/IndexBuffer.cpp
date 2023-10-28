#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(void* data, uint32_t size): m_Size(size)
{
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void IndexBuffer::SetData(uint32_t* data, uint32_t size)
{
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    m_Size = size;
}

void IndexBuffer::SetData(uint32_t* data)
{
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, data, GL_STATIC_DRAW);
}

void IndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void IndexBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t IndexBuffer::GetCount()
{
    return m_Size / sizeof(uint32_t);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_Id);
}
