#include "Texture2D.h"
#include "Core/Ed.h"

uint32_t Texture2D::GetWidth() const
{
	return GetDescriptor<Texture2DDescriptor>()->Data.GetWidth();
}

uint32_t Texture2D::GetHeight() const
{
	return GetDescriptor<Texture2DDescriptor>()->Data.GetHeight();
}

glm::u32vec3 Texture2D::GetSize() const
{
	Texture2DData& data = GetDescriptor<Texture2DDescriptor>()->Data;
	return { data.GetWidth(), data.GetHeight(), 0 };
}

void Texture2D::Resize(uint32_t width, uint32_t height, uint32_t depth)
{
	Resize(width, height);
}

void Texture2D::Resize(glm::u32vec3 size)
{
	Resize(size.x, size.y);
}

Texture2D::Texture2D(std::shared_ptr<Texture2DDescriptor> descriptor) : Texture(descriptor)
{
}
