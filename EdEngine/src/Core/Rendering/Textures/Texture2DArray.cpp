#include "Texture2DArray.h"

uint32_t Texture2DArray::GetWidth() const
{
    return GetDescriptor<Texture2DArrayDescriptor>()->Data.GetWidth();
}

uint32_t Texture2DArray::GetHeight() const
{
	return GetDescriptor<Texture2DArrayDescriptor>()->Data.GetHeight();
}

uint32_t Texture2DArray::GetDepth() const
{
	return GetDescriptor<Texture2DArrayDescriptor>()->Data.GetDepth();
}

glm::u32vec3 Texture2DArray::GetSize() const
{
	Texture2DArrayData& data = GetDescriptor<Texture2DArrayDescriptor>()->Data;
	return { data.GetWidth(), data.GetHeight(), data.GetDepth() };
}

TextureType Texture2DArray::GetType() const
{
    return TextureType::Texture2DArray;
}

void Texture2DArray::Resize(glm::u32vec3 size)
{
	Resize(size.x, size.y, size.z);
}

void Texture2DArray::Resize(uint32_t width, uint32_t height)
{
	Resize(width, height, GetDepth());
}

Texture2DArray::Texture2DArray(std::shared_ptr<Texture2DArrayDescriptor> descriptor) : Texture(descriptor)
{
}
