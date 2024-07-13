#pragma once

#include "Buffer.h"
#include "VertexBufferLayout.h"

class VertexBuffer : public Buffer
{
public:
	VertexBuffer(const std::string& name);

	virtual void SetLayout(const VertexBufferLayout& layout) = 0;
	
	const VertexBufferLayout& GetLayout() const;
	
	virtual uint32_t GetCount() const = 0;

	virtual ~VertexBuffer() = default;
protected:
	VertexBufferLayout m_Layout;
};

