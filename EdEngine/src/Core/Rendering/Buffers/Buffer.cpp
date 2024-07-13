#include "Buffer.h"

Buffer::Buffer(const std::string& name) : Resource(name)
{
}

uint32_t Buffer::GetSize() const
{
  return m_Size;
}
