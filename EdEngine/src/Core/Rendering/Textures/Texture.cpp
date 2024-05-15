#include "Texture.h"
#include "Core/Assets/ImportParameters/TextureImportParameters.h"

Texture::Texture(const std::string& name) : Super(name), Resource( { name } ) // TODO : Fix it :)
{
}

uint32_t Texture::GetID() const
{
	return m_Id;
}

void Texture::SetWrapS(WrapMode mode)
{
	m_WrapS = mode;
	MarkDirty();
	RefreshParameters();
}

WrapMode Texture::GetWrapS() const
{
	return m_WrapS;
}

void Texture::SetWrapT(WrapMode mode)
{
	m_WrapT = mode;
	MarkDirty();
	RefreshParameters();
}

WrapMode Texture::GetWrapT() const
{
	return m_WrapT;
}

void Texture::SetPixelFormat(PixelFormat format)
{
	m_PixelFormat = format;
	MarkDirty();
	RefreshData();
}

PixelFormat Texture::GetPixelFormat() const
{
	return m_PixelFormat;
}

void Texture::SetFilteringMode(FilteringMode mode)
{
	m_FilteringMode = mode;
	MarkDirty();
	RefreshParameters();
}

FilteringMode Texture::GetFilteringMode() const
{
	return m_FilteringMode;
}

void Texture::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & m_WrapS;
	archive & m_WrapT;
	archive & m_PixelFormat;
	archive & m_FilteringMode;
}

void Texture::ResetState()
{
	Super::ResetState();

	std::shared_ptr<TextureImportParameters> paramters = std::static_pointer_cast<TextureImportParameters>(m_ImportParameters);

	m_WrapS = paramters->WrapS;
	m_WrapT = paramters->WrapT;
	m_PixelFormat = paramters->Format;
	m_FilteringMode = paramters->Filtering;

	if (m_bIsInitialized)
	{
		RefreshParameters();
		MarkDirty();
	}
}

bool Texture::IsInitialized() const
{
	return m_bIsInitialized;
}
