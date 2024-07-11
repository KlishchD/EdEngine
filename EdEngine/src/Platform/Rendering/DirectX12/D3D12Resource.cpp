#include "D3D12Resource.h"
#include "D3D12RenderingContext.h"

void D3D12Resource::SetDescription(const D3D12_RESOURCE_DESC& description)
{
  m_Description = description;
}

void D3D12Resource::SetResourceData(void* data, uint32_t size, uint32_t offset)
{
  ED_ASSERT(m_Description.Width != 0, "This method can only be used after descriptor was provided.");

  gContext->AddResourceForUploading(data, size, m_Description.Width == 0 ? nullptr : &m_Description, this);
}

void D3D12Resource::SetResourceData(void* data, uint32_t size, uint32_t offset, const D3D12_RESOURCE_DESC& description)
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

