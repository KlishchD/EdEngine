#include "Texture.h"

Texture::Texture(std::shared_ptr<AssetDescriptor> descriptor)
{
	SetDescriptor(descriptor);
}

uint32_t Texture::GetID() const
{
	return m_Id;
}

PixelFormat Texture::GetPixelFormat() const
{
	return GetDescriptor<TextureDescriptor>()->GetImportParameters()->Format;
}

void Texture::Initialize()
{
}