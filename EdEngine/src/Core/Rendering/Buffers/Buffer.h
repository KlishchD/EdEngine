#pragma once

#include <cstdint>
#include "Core/Rendering/Types.h"

class Buffer {
public:
	virtual void SetData(void* data, BufferUsage usage) = 0;
	virtual void SetData(void* data, int32_t size, BufferUsage usage) = 0;

	virtual void SetSubdata(uint32_t offset, uint32_t size, void* data) = 0;

	virtual ~Buffer() = default;
};