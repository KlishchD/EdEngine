#include "D3D12Resource.h"
#include "D3D12RenderingContext.h"

void D3D12Resource::SetDescription(const D3D12_RESOURCE_DESC1& description)
{
  m_Description = description;
  m_TotalSize = D3D12Helper::CalculateResourceSize(description);
  ED_ASSERT(m_TotalSize, "Incorrect resource size.");
}

void D3D12Resource::SetResourceData(void* data, uint32_t size, uint32_t offset)
{
  ED_ASSERT(offset + size <= m_TotalSize, "This resource is allocated for {} and was provided from {} to {} bytes of data.", m_TotalSize, offset,  offset + size);
  ED_ASSERT(m_Description.Width != 0, "This method can only be used after descriptor was provided.");

  gContext->AddResourceForUploading(data, size, nullptr, this);
}

void D3D12Resource::SetResourceData(void* data, uint32_t size, uint32_t offset, D3D12_RESOURCE_DESC1 description)
{
  SetDescription(description);
  SetResourceData(data, size, offset);
}

void D3D12Resource::SetResource(Microsoft::WRL::ComPtr<ID3D12Resource1> resource)
{
  m_Resource = resource;
}

Microsoft::WRL::ComPtr<ID3D12Resource1> D3D12Resource::GetResource() const
{
  return m_Resource;
}

