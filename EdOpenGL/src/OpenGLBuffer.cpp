#include "EdOpenGL.h"
#include "Buffer.h"

Buffer::Buffer(BufferType type, u32 initialSize, BufferUsage usage) : m_Type(type), m_Size(initialSize), m_Usage(usage)
{
    extern uptr GetNextHandle(OpenGLHandleType type);
    m_NativeHandle = GetNextHandle(OpenGLHandleType::Buffer);
}

void Buffer::SetDebugName(ccstr8 name, i32 size)
{
    glObjectLabel(GL_BUFFER, m_NativeHandle, size, name);
}

void Buffer::GetDebugName(ccstr8& name, i32& size)
{
    ED_ASSERT(size, "Expected size to be string capacity, e.g size > 0.");
    glGetObjectLabel(GL_BUFFER, m_NativeHandle, size, &size, const_cast<c8*>(name));
}

// Note (for me future): this could fail for uniform buffer
void Buffer::SetData(void* data)
{
    ED_ASSERT(m_NativeHandle, "Attemped to set data for not initialzed buffer.");
    ED_ASSERT(data, "Attemped to set null data.");
    glNamedBufferData(m_NativeHandle, m_Size, data, OpenGLTypes::ConvertBufferUsage(m_Usage));
}

void Buffer::SetData(void* data, i32 size)
{
    ED_ASSERT(m_NativeHandle, "Attemped to set data for not initialzed buffer.");
    ED_ASSERT(data, "Attemped to set null data.");

    m_Size = size;
    glNamedBufferData(m_NativeHandle, m_Size, data, OpenGLTypes::ConvertBufferUsage(m_Usage));
}

void Buffer::SetSubdata(u32 offset, u32 size, void* data)
{
    ED_ASSERT(m_NativeHandle, "Attemped to set data for not initialzed buffer.");
    ED_ASSERT(offset + size <= m_Size, "Attemted to put data outside of the buffer.");
    ED_ASSERT(data, "Attemped to set null data.");
    glNamedBufferSubData(m_NativeHandle, offset, size, data);
}

Buffer::~Buffer()
{
    extern void ReturnHandle(OpenGLHandleType type, uptr Handle);
    ReturnHandle(OpenGLHandleType::Buffer, m_NativeHandle);
}
