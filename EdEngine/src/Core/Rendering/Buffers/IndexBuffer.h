#pragma once

#include "Buffer.h"

class IndexBuffer: public Buffer
{
public:
  IndexBuffer(const std::string& name);

  virtual uint32_t GetCount() = 0;
};
