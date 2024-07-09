#pragma once

#include "EdD3D12Rendering.h"

class D3D12Resource
{
protected:
  friend class D3D12RenderingContext;

  void SetDescription(const D3D12_RESOURCE_DESC1& description);

  void SetResourceData(void* data, uint32_t size, uint32_t offset);
  void SetResourceData(void* data, uint32_t size, uint32_t offset, D3D12_RESOURCE_DESC1 description);

  void SetResource(Microsoft::WRL::ComPtr<ID3D12Resource1> resource);
  Microsoft::WRL::ComPtr<ID3D12Resource1> GetResource() const;
protected:
  Microsoft::WRL::ComPtr<ID3D12Resource1> m_Resource;
  
  D3D12_RESOURCE_DESC1 m_Description{};

  uint32_t m_UsedSpace = 0;
  uint32_t m_TotalSize = 0;
};