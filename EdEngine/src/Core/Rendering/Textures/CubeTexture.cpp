#include "CubeTexture.h"
#include "Core/Assets/ImportParameters/TextureImportParameters.h"

CubeTexture::CubeTexture(const std::string& name) : Super(name)
{
}

AssetType CubeTexture::GetType() const
{
	return AssetType::CubeTexture;
}

glm::u32vec3 CubeTexture::GetSize() const
{
    uint32_t size = m_Data.GetSize();
    return { size, size, size };
}

void CubeTexture::SetWrapR(WrapMode mode)
{
    m_WrapR = mode;
	RefreshParameters();
}

WrapMode CubeTexture::GetWrapMode() const
{
    return m_WrapR;
}

void CubeTexture::SetData(const CubeTextureData& data)
{
	m_Data = data;
	MarkDirty();
	RefreshData();
}

void CubeTexture::SetData(CubeTextureData&& data)
{
	m_Data = std::move(data);
	MarkDirty();
	RefreshData();
}

TextureType CubeTexture::GetTextureType() const
{
	return TextureType::CubeTexture;
}

void CubeTexture::Resize(uint32_t width, uint32_t height, uint32_t depth)
{
    Resize(width);
}

void CubeTexture::Resize(glm::u32vec3 size)
{
    Resize(size.x);
}

void CubeTexture::Resize(uint32_t size)
{
	if (m_Data.GetSize() != size)
	{
		m_Data.SetData(nullptr, 0, true);
		m_Data.SetSize(size);

		MarkDirty();
		RefreshData();
	}
}

void CubeTexture::ResetState()
{
	std::shared_ptr<CubeTextureImportParameters> paramters = std::static_pointer_cast<CubeTextureImportParameters>(m_ImportParameters);
	m_WrapR = paramters->WrapR;

	Texture::ResetState();
}

void CubeTexture::Serialize(Archive& archive)
{
	Texture::Serialize(archive);

	archive & m_WrapR;
}

void CubeTexture::SerializeData(Archive& archive)
{
	Texture::SerializeData(archive);

	archive & m_Data;

    if (archive.GetMode() == ArchiveMode::Read)
    {
        RefreshParameters();
        RefreshData();
    }
}
