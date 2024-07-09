#pragma once

#include "Core/Rendering/Buffers/VertexBuffer.h"
#include "Platform/Rendering/DirectX12/D3D12Resource.h"

class D3D12VertexBuffer : public VertexBuffer, public D3D12Resource
{
public:
	virtual void SetLayout(const VertexBufferLayout& layout) override;
	virtual uint32_t GetCount() const override;

	virtual void SetData(void* data, BufferUsage usage) override;
	virtual void SetData(void* data, int32_t size, BufferUsage usage) override;

	virtual void SetSubdata(uint32_t offset, uint32_t size, void* data) override;
};