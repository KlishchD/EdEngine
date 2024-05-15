#include "Texture2DArray.h"
#include "Core/Assets/ImportParameters/TextureImportParameters.h"

Texture2DArray::Texture2DArray(const std::string& name) : Super(name)
{
}

AssetType Texture2DArray::GetType() const
{
	return AssetType::Texture2DArray;
}

uint32_t Texture2DArray::GetWidth() const
{
	return m_Data.GetWidth();
}

uint32_t Texture2DArray::GetHeight() const
{
	return m_Data.GetHeight();
}

uint32_t Texture2DArray::GetDepth() const
{
	return m_Data.GetDepth();
}

glm::u32vec3 Texture2DArray::GetSize() const
{
	return { m_Data.GetWidth(), m_Data.GetHeight(), m_Data.GetDepth() };
}

void Texture2DArray::SetData(const Texture2DArrayData& data)
{
	m_Data = data;
	MarkDirty();
	RefreshData();
}

void Texture2DArray::SetData(Texture2DArrayData&& data)
{
	m_Data = std::move(data);
	MarkDirty();
	RefreshData();
}

TextureType Texture2DArray::GetTextureType() const
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

void Texture2DArray::Resize(uint32_t width, uint32_t height, uint32_t depth)
{
	if (m_Data.GetWidth() != width || m_Data.GetHeight() != height || m_Data.GetDepth() != depth)
	{
		m_Data.SetData(nullptr, 0, true);
		m_Data.SetWidth(width);
		m_Data.SetHeight(height);
		m_Data.SetDepth(depth);

		MarkDirty();
		RefreshData();
	}
}

void Texture2DArray::SerializeData(Archive& archive)
{
	Super::SerializeData(archive);

	archive & m_Data;

    if (archive.GetMode() == ArchiveMode::Read)
    {
        RefreshParameters();
        RefreshData();
    }
}
