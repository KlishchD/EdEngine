#include "Texture2D.h"
#include "Core/Assets/ImportParameters/TextureImportParameters.h"

Texture2D::Texture2D(const std::string& name) : Super(name)
{
}

AssetType Texture2D::GetType() const
{
	return AssetType::Texture2D;
}

uint32_t Texture2D::GetWidth() const
{
	return m_Data.GetWidth();
}

uint32_t Texture2D::GetHeight() const
{
	return m_Data.GetHeight();
}

glm::u32vec3 Texture2D::GetSize() const
{
	return { m_Data.GetWidth(), m_Data.GetHeight(), 0 };
}

void Texture2D::SetData(const Texture2DData& data)
{
	m_Data = data;
	MarkDirty();
	RefreshData();
}

void Texture2D::SetData(Texture2DData&& data)
{
	m_Data = std::move(data);
	MarkDirty();
	RefreshData();
}

void Texture2D::SetMipMapsEnabled(bool enabled)
{
	m_bMipMapsEnabled = enabled;

	MarkDirty();
	if (m_bIsInitialized)
	{
		if (m_bMipMapsEnabled)
		{
			GenerateMipMaps();
		}
		else
		{
			DeleteMipMaps();
		}
	}
}

bool Texture2D::AreMipMapsEnabled() const
{
	return m_bMipMapsEnabled;
}

void Texture2D::Resize(uint32_t width, uint32_t height, uint32_t depth)
{
	Resize(width, height);
}

void Texture2D::Resize(glm::u32vec3 size)
{
	Resize(size.x, size.y);
}

void Texture2D::Resize(uint32_t width, uint32_t height)
{
	if (m_Data.GetWidth() != width || m_Data.GetHeight() != height)
	{
		m_Data.SetData(nullptr, 0, true);
		m_Data.SetWidth(width);
		m_Data.SetHeight(height);

		MarkDirty();
		RefreshData();
	}
}

void Texture2D::ResetState()
{
	Super::ResetState();

	std::shared_ptr<Texture2DImportParameters> paramters = std::static_pointer_cast<Texture2DImportParameters>(m_ImportParameters);
	SetMipMapsEnabled(paramters->GenerateMipMaps);
}

void Texture2D::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & m_bMipMapsEnabled;
}

void Texture2D::SerializeData(Archive& archive)
{
	Super::SerializeData(archive);

	archive & m_Data;

	if (archive.GetMode() == ArchiveMode::Read)
	{
		if (m_bIsInitialized)
		{
			RefreshParameters();
			RefreshData();
		}
		else
		{
			Initialize();
		}
	}
}

TextureType Texture2D::GetTextureType() const
{
	return TextureType::Texture2D;
}
