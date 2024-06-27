#pragma once

#include "Core/Rendering/Buffers/VertexBuffer.h"

class OpenGLVertexBuffer : public VertexBuffer
{
public:
	OpenGLVertexBuffer();

	virtual void SetLayout(const VertexBufferLayout& layout) override;

	virtual void SetData(void* data, BufferUsage usage) override;
	virtual void SetData(void* data, int32_t size, BufferUsage usage) override;

	virtual void SetSubdata(uint32_t offset, uint32_t size, void* data) override;

	virtual uint32_t GetCount() const override;

	uint32_t GetID() const;

	~OpenGLVertexBuffer();
protected:
	uint32_t m_Id = 0;
	uint32_t m_VertexSize;
};