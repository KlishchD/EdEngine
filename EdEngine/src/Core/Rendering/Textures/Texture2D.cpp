#include "Texture2D.h"
#include "Core/Ed.h"

uint32_t Texture2D::GetWidth() const
{
	Texture2DData& data = (Texture2DData&)GetData();
	return data.Width;
}

uint32_t Texture2D::GetHeight() const
{
	Texture2DData& data = (Texture2DData&)GetData();
	return data.Height;
}

Texture2D::Texture2D(const std::shared_ptr<TextureDescriptor>& descriptor) : Texture(descriptor)
{
}