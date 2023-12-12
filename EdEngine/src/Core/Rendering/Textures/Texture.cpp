#include "Texture.h"

Texture::Texture(const std::shared_ptr<TextureDescriptor>& descriptor)
{
	SetDescriptor(descriptor);
}

uint32_t Texture::GetID() const
{
	return m_Id;
}

const TextureImportParameters& Texture::GetImportParameters() const
{
	return *GetDescriptor<TextureDescriptor>()->GetImportParameters();
}

TextureData& Texture::GetData() const
{
	return *GetDescriptor<TextureDescriptor>()->GetData();
}

void Texture::Initialize()
{
	Initialize(GetDescriptor<TextureDescriptor>()->GetImportParameters(), GetDescriptor<TextureDescriptor>()->GetData());
}

void Texture::Initialize(TextureImportParameters* parameters, TextureData* data)
{
}
