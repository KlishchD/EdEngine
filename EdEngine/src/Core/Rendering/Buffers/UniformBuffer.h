#pragma once

#include "Buffer.h"

class UniformBuffer : public Buffer
{
public:
	template<typename T>
	void SetDataFromObject(T* data, BufferUsage usage)
	{
		SetData(data, sizeof(T), usage);
	}
};