#pragma once

#include "Core/Rendering/EdRendering.h"

class Buffer : public Resource
{
public:
  Buffer(const std::string& name);

  virtual void SetData(void* data, BufferUsage usage) = 0;
  virtual void SetData(void* data, int32_t size, BufferUsage usage) = 0;

  virtual void SetSubdata(uint32_t offset, uint32_t size, void* data) = 0;

  uint32_t GetSize() const;

  virtual ~Buffer() = default;

protected:
  uint32_t m_Size = 0;
};
