#pragma once

#include "Platform/Rendering/DirectX12/EdD3D12Rendering.h"
#include "Core/Rendering/Buffers/VertexBuffer.h"

class D3D12VertexBuffer : public VertexBuffer
{
public:
  D3D12VertexBuffer(const std::string& name);

  virtual void SetLayout(const VertexBufferLayout& layout) override;
  virtual uint32_t GetCount() const override;

  virtual void SetData(void* data, BufferUsage usage) override;
  virtual void SetData(void* data, int32_t size, BufferUsage usage) override;

  virtual void SetSubdata(uint32_t offset, uint32_t size, void* data) override;

  virtual void* GetNativeResource() const override;
  virtual void SetNativeResource(void* resource) override;
protected:
  Microsoft::WRL::ComPtr<ID3D12Resource1> m_Resource;
  D3D12_RESOURCE_DESC m_Description {};
};