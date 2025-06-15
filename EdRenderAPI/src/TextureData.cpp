#include "EdRenderApiPrivate.h"
#include "TextureData.h"

Texture2DData::Texture2DData() : TextureData(TextureType::Texture2D, nullptr, 0, false), m_Width(1), m_Height(1)
{
}

Texture2DData::Texture2DData(i32 width, i32 height) : TextureData(TextureType::Texture2D, nullptr, 0, false), m_Width(width), m_Height(height)
{
}

Texture2DData::Texture2DData(i32 width, i32 height, u8* data, u32 size, bool bTakeOwnership) : TextureData(TextureType::Texture2D, data, size, bTakeOwnership), m_Width(width), m_Height(height)
{
}

Texture2DData& Texture2DData::operator=(const Texture2DData& data)
{
	TextureData::operator=(data);

	m_Width = data.m_Width;
	m_Height = data.m_Height;

	return *this;
}

Texture2DData& Texture2DData::operator=(Texture2DData&& data)
{
	TextureData::operator=(std::move(data));

	m_Width = data.m_Width;
	m_Height = data.m_Height;

	data.m_Width = 0;
	data.m_Height = 0;

	return *this;
}

void Texture2DData::SetWidth(u32 width)
{
	m_Width = width;
}

u32 Texture2DData::GetWidth() const
{
	return m_Width;
}

void Texture2DData::SetHeight(u32 height)
{
	m_Height = height;
}

u32 Texture2DData::GetHeight() const
{
	return m_Height;
}

void Texture2DData::SetSize(glm::u32vec2 size)
{
	m_Width = size.x;
	m_Height = size.y;
}

glm::u32vec2 Texture2DData::GetSize() const
{
	return { m_Width, m_Height };
}

void Texture2DData::Serialize(Archive<>& archive)
{
	TextureData::Serialize(archive);

    archive & m_Width;
    archive & m_Height;
}

TextureData::TextureData(TextureType type) : m_Data(nullptr), m_DataSize(0), m_bDataOwner(true), m_Type(type)
{
}

TextureData::TextureData(TextureType type, u8* data, u32 size, bool bTakeOwnership) : m_DataSize(size), m_Data(data), m_bDataOwner(bTakeOwnership), m_Type(type)
{
}

void TextureData::Serialize(Archive<>& archive)
{
	archive.Serialize<u8, false>(m_Data, m_DataSize);

	if (archive.GetMode() == SerializationMode::Read)
	{
		m_bDataOwner = true;
	}
}

TextureData& TextureData::operator=(const TextureData& data)
{
	FreeData();

	m_DataSize = data.m_DataSize;
	m_bDataOwner = data.m_bDataOwner;

	if (data.m_bDataOwner)
	{
		m_Data = (u8*)malloc(m_DataSize);
		memcpy(m_Data, data.m_Data, m_DataSize);
	}
	else
	{
		m_DataSize = data.m_DataSize;
		m_Data = data.m_Data;
	}

	return *this;
}

TextureData& TextureData::operator=(TextureData&& data)
{
	FreeData();

	m_DataSize = data.m_DataSize;
	m_Data = data.m_Data;

	m_bDataOwner = data.m_bDataOwner;

	data.m_Data = nullptr;
	data.m_DataSize = 0;

	return *this;
}

void TextureData::SetData(u8* data, u32 size, bool bTakeOwnership)
{
	FreeData();

	m_Data = data;
	m_DataSize = size;
	m_bDataOwner = bTakeOwnership;
}

TextureData::~TextureData()
{
	FreeData();
}

void TextureData::FreeData()
{
	if (m_Data)
	{
		if (m_bDataOwner)
		{
			delete[] m_Data;
		}

		m_Data = nullptr;
		m_DataSize = 0;
	}
}

Texture2DArrayData::Texture2DArrayData() : TextureData(TextureType::TextureArray, nullptr, 0, false), m_Width(1), m_Height(1), m_Depth(1)
{
}

Texture2DArrayData::Texture2DArrayData(i32 width, i32 height, i32 depth) : TextureData(TextureType::TextureArray, nullptr, 0, false), m_Width(width), m_Height(height), m_Depth(depth)
{
}

Texture2DArrayData::Texture2DArrayData(i32 width, i32 height, i32 depth, u8* data, u32 size, bool bTakeOwnership) : TextureData(TextureType::TextureArray, data, size, bTakeOwnership), m_Width(width), m_Height(height), m_Depth(depth)
{
}

Texture2DArrayData& Texture2DArrayData::operator=(const Texture2DArrayData& data)
{
	TextureData::operator=(data);

	m_Width = data.m_Width;
	m_Height = data.m_Height;
	m_Depth = data.m_Depth;

	return *this;
}

Texture2DArrayData& Texture2DArrayData::operator=(Texture2DArrayData&& data)
{
	TextureData::operator=(std::move(data));

	m_Width = data.m_Width;
	m_Height = data.m_Height;
	m_Depth = data.m_Depth;

	data.m_Width = 0;
	data.m_Height = 0;
	data.m_Depth = 0;

	return *this;
}

void Texture2DArrayData::SetWidth(u32 width)
{
	m_Width = width;
}

u32 Texture2DArrayData::GetWidth() const
{
	return m_Width;
}

void Texture2DArrayData::SetHeight(u32 height)
{
	m_Height = height;
}

u32 Texture2DArrayData::GetHeight() const
{
	return m_Height;
}

void Texture2DArrayData::SetDepth(u32 depth)
{
	m_Depth = depth;
}

u32 Texture2DArrayData::GetDepth() const
{
	return m_Depth;
}

void Texture2DArrayData::SetSize(glm::u32vec3 size)
{
	m_Width = size.x;
	m_Height = size.y;
	m_Depth = size.z;
}

glm::u32vec3 Texture2DArrayData::GetSize() const
{
	return { m_Width, m_Height, m_Depth };
}

void Texture2DArrayData::Serialize(Archive<>& archive)
{
	TextureData::Serialize(archive);

    archive & m_Width;
    archive & m_Height;
    archive & m_Depth;
}
