#include "UploadBuffer.h"

UploadBuffer::UploadBuffer(uint32_t size) : m_Size(size)
{
  m_Memory = (uint8_t*)malloc(m_Size);
  ED_ASSERT(m_Memory, "Failed to allocate {}", m_Size);
  m_FreeStart = m_Memory;
}

UploadBuffer::UploadBuffer(const UploadBuffer& buffer)
{
  m_Memory = (uint8_t*) malloc(buffer.m_Size);
  ED_ASSERT(m_Memory, "Failed to allocate {}", m_Size);

  memcpy(m_Memory, buffer.m_Memory, m_Size);
  m_FreeStart = m_Memory + (buffer.m_FreeStart - buffer.m_Memory);

  m_Size = buffer.m_Size;

  m_IsOwner = true;
}

UploadBuffer::UploadBuffer(UploadBuffer&& buffer) : m_Memory(buffer.m_Memory), m_FreeStart(buffer.m_FreeStart), m_Size(buffer.m_Size), m_IsOwner(buffer.m_IsOwner) 
{
  buffer.m_Memory = nullptr;
  buffer.m_FreeStart = nullptr;
  buffer.m_Size = 0;
  buffer.m_IsOwner = false;
}

UploadBuffer UploadBuffer::operator=(UploadBuffer&& buffer)
{
  return UploadBuffer(std::move(buffer));
}

UploadBuffer UploadBuffer::operator=(UploadBuffer& buffer)
{
  return UploadBuffer(buffer);
}

void UploadBuffer::relinquishData()
{
  m_IsOwner = false;
}

uint8_t* UploadBuffer::GetBegin() const
{
  return m_Memory;
}

uint8_t* UploadBuffer::GetEnd() const
{
  return m_FreeStart;
}

uint32_t UploadBuffer::GetOccupiedSize() const
{
  return m_FreeStart - m_Memory;
}

uint32_t UploadBuffer::GetTotalSize() const
{
  return m_Size;
}

UploadBuffer::~UploadBuffer()
{
  if (m_IsOwner)
  {
    delete[] m_Memory;
  }
}

void UploadBuffer::Expand()
{
  uint32_t newSize = m_Size * 2;

  uint8_t* newPtr = (uint8_t*)malloc(newSize);
  ED_ASSERT(newPtr, "Failed to allocate {}", newSize);

  memcpy(newPtr, m_Memory, m_Size);

  m_FreeStart = newPtr + (m_Memory + m_Size - m_FreeStart);

  delete[] m_Memory;
  m_Memory = newPtr;

  m_Size = newSize;
}
