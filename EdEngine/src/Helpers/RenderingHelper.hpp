#include "RenderingHelper.h"
#include "Core/Rendering/Buffers/UniformBuffer.h"

template<typename T>
static std::shared_ptr<UniformBuffer> RenderingHelper::CreateUniformBuffer(T* data, BufferUsage usage)
{
	std::shared_ptr<UniformBuffer> buffer = CreateUniformBuffer();
	buffer->SetDataFromObject<T>(data, usage);
	return buffer;
}