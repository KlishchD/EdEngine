#pragma once

#include <memory>

#include "VertexBufferLayout.h"

class VertexBuffer
{
public:
	VertexBuffer(void* data, int32_t size, const VertexBufferLayout& layout);
	VertexBuffer(void* data, int32_t size);
	
	void SetLayout(const VertexBufferLayout& layout);
	void SetData(void* data);
	void SetData(void* data, int32_t size);
	
	const VertexBufferLayout& GetLayout() const;
	uint32_t GetStride() const;

	void Bind() const;
	void Unbind() const;
	void SetSubData(uint32_t offset, uint32_t size, void* data);

	static std::shared_ptr<VertexBuffer> GetCubeVertexBuffer();

	~VertexBuffer();
private:
	uint32_t m_Id;
	int32_t m_Size;
	VertexBufferLayout m_Layout;
};

