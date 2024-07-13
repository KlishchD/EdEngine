#pragma once

#include "Buffer.h"

class UniformBuffer : public Buffer
{
public:
	UniformBuffer(const std::string& name);

	template<typename T>
	void SetDataFromObject(T* data, BufferUsage usage)
	{
		SetData(data, sizeof(T), usage);
	}
};