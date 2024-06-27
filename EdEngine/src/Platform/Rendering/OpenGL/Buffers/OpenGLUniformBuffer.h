#pragma once

#include "Core/Rendering/Buffers/UniformBuffer.h"

class OpenGLUniformBuffer : public UniformBuffer
{
public:
	OpenGLUniformBuffer();

	virtual void SetData(void* data, BufferUsage usage) override;
	virtual void SetData(void* data, int32_t size, BufferUsage usage) override;

	virtual void SetSubdata(uint32_t offset, uint32_t size, void* data) override;

	uint32_t GetID() const;

	virtual ~OpenGLUniformBuffer() override;
protected:
	uint32_t m_Id = 0;
};