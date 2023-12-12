#pragma once

#include <memory>
#include "Buffer.h"
#include "Core/Rendering/Types.h"
#include "VertexBufferLayout.h"

class VertexBuffer : public Buffer
{
public:
	virtual void SetLayout(const VertexBufferLayout& layout) = 0;
	
	const VertexBufferLayout& GetLayout() const;
	
	virtual uint32_t GetCount() const = 0;

	virtual ~VertexBuffer() = default;
protected:
	VertexBufferLayout m_Layout;
};

