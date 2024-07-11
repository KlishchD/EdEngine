#include "D3D12VertexBuffer.h"

void D3D12VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{

}

uint32_t D3D12VertexBuffer::GetCount() const
{
	return m_Size;
}

void D3D12VertexBuffer::SetData(void* data, BufferUsage usage)
{
	SetResourceData(data, m_Size, 0);
}

void D3D12VertexBuffer::SetData(void* data, int32_t size, BufferUsage usage)
{
	if (size >= m_Size)
	{
		D3D12_RESOURCE_DESC description;
		description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		description.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		description.Width = size;
		description.Height = 1;
		description.DepthOrArraySize = 1;
		description.MipLevels = 1;
		description.Format = DXGI_FORMAT_UNKNOWN;
		description.SampleDesc.Count = 1;
		description.SampleDesc.Quality = 0;
		description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		description.Flags = D3D12_RESOURCE_FLAG_NONE;
		SetDescription(description);
	}
	SetResourceData(data, size, 0);
	m_Size = size;
}

void D3D12VertexBuffer::SetSubdata(uint32_t offset, uint32_t size, void* data)
{
	SetResourceData(data, size, offset);
}
