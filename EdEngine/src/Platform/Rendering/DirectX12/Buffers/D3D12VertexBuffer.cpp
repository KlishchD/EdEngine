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
	SetResourceData(data, m_TotalSize, 0);
	m_Size = m_TotalSize;
}

void D3D12VertexBuffer::SetData(void* data, int32_t size, BufferUsage usage)
{
	if (size >= m_TotalSize)
	{
		D3D12_RESOURCE_DESC1 description;
		description.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		description.Width = size;
		description.Height = 1;
		description.DepthOrArraySize = 1;
		description.MipLevels = 1;
		description.Format = DXGI_FORMAT_R32_FLOAT;
		description.SampleDesc.Count = 1;
		description.SampleDesc.Quality = 0;
		description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		description.Flags = D3D12_RESOURCE_FLAG_NONE;
		description.SamplerFeedbackMipRegion.Width = 0;
		description.SamplerFeedbackMipRegion.Height = 0;
		description.SamplerFeedbackMipRegion.Depth = 0;
		SetDescription(description);
	}
	SetResourceData(data, size, 0);
	m_Size = size;
}

void D3D12VertexBuffer::SetSubdata(uint32_t offset, uint32_t size, void* data)
{
	SetResourceData(data, size, offset);
	m_Size = size;
}
