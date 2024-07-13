#pragma once

#include "Core/Rendering/Buffers/IndexBuffer.h"

class OpenGLIndexBuffer : public IndexBuffer
{
public:
  OpenGLIndexBuffer(const std::string& name);

  virtual void SetData(void* data, BufferUsage usage) override;
  virtual void SetData(void* data, int32_t size, BufferUsage usage) override;

  virtual void SetSubdata(uint32_t offset, uint32_t size, void* data) override;

  virtual uint32_t GetCount() override;

  virtual void* GetNativeResource() const override;
  virtual void SetNativeResource(void* resource) override;

  virtual ~OpenGLIndexBuffer() override;

private:
  uint32_t m_Id = 0;
};
