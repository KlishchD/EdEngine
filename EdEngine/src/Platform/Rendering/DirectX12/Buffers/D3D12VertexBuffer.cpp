#include "D3D12VertexBuffer.h"
#include "Helpers/StringHelper.h"
#include "Platform/Rendering/DirectX12/D3D12RenderingContext.h"

D3D12VertexBuffer::D3D12VertexBuffer(const std::string& name) : VertexBuffer(name)
{
}

void D3D12VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
}

uint32_t D3D12VertexBuffer::GetCount() const
{
  return m_Size;
}

void D3D12VertexBuffer::SetData(void* data, BufferUsage usage)
{
  gContext->AddResourceForUploading(data, m_Size, 0, m_Description.Width == 0 ? nullptr : &m_Description, this);
}

void D3D12VertexBuffer::SetData(void* data, int32_t size, BufferUsage usage)
{
  bool bMustCreateNewResourceDestination = false;

  if (size >= m_Size)
  {
    m_Description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    m_Description.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    m_Description.Width = size;
    m_Description.Height = 1;
    m_Description.DepthOrArraySize = 1;
    m_Description.MipLevels = 1;
    m_Description.Format = DXGI_FORMAT_UNKNOWN;
    m_Description.SampleDesc.Count = 1;
    m_Description.SampleDesc.Quality = 0;
    m_Description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    m_Description.Flags = D3D12_RESOURCE_FLAG_NONE;
    bMustCreateNewResourceDestination = true;
  }

  gContext->AddResourceForUploading(data, size, 0, bMustCreateNewResourceDestination ? &m_Description : nullptr, this);
  m_Size = size;
}

void D3D12VertexBuffer::SetSubdata(uint32_t offset, uint32_t size, void* data)
{
  ED_ASSERT(offset + size < m_Size, "Provided more data than buffer can store");

  gContext->AddResourceForUploading(data, m_Size, offset, nullptr, this);
}

void* D3D12VertexBuffer::GetNativeResource() const
{
  return reinterpret_cast<void*>(m_Resource.Get());
}

void D3D12VertexBuffer::SetNativeResource(void* resource)
{
  m_Resource = reinterpret_cast<ID3D12Resource1*>(resource);
  if (m_Resource)
  {
    m_Resource->SetName(StringHelper::StringToWCHAR(m_Name).data());
  }
}
