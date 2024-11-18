#include "TextureData.h"

Texture2DData::Texture2DData() : TextureData(nullptr, 0, false), m_Width(1), m_Height(1)
{
}

Texture2DData::Texture2DData(int32_t width, int32_t height) : TextureData(nullptr, 0, false), m_Width(width), m_Height(height)
{
}

Texture2DData::Texture2DData(int32_t width, int32_t height, uint8_t* data, uint32_t size, bool bTakeOwnership) : TextureData(data, size, bTakeOwnership), m_Width(width), m_Height(height)
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

void Texture2DData::SetWidth(uint32_t width)
{
	m_Width = width;
}

uint32_t Texture2DData::GetWidth() const
{
	return m_Width;
}

void Texture2DData::SetHeight(uint32_t height)
{
	m_Height = height;
}

uint32_t Texture2DData::GetHeight() const
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

void Texture2DData::Serialize(Archive& archive)
{
	TextureData::Serialize(archive);

    archive & m_Width;
    archive & m_Height;
}

TextureData::TextureData() : m_Data(nullptr), m_DataSize(0), m_bDataOwner(true)
{
}

TextureData::TextureData(uint8_t* data, uint32_t size, bool bTakeOwnership) : m_DataSize(size), m_Data(data), m_bDataOwner(bTakeOwnership)
{
}

void TextureData::Serialize(Archive& archive)
{
	Serializable::Serialize(archive);

	archive.Serialize<uint8_t>(m_Data, m_DataSize);

	if (archive.GetMode() == ArchiveMode::Read)
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
		m_Data = (uint8_t*)malloc(m_DataSize);
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

void TextureData::SetData(uint8_t* data, uint32_t size, bool bTakeOwnership)
{
	FreeData();

	m_Data = data;
	m_DataSize = size;
	m_bDataOwner = bTakeOwnership;
}

uint8_t* TextureData::GetData() const
{
	return m_Data;
}

uint32_t TextureData::GetDataSize() const
{
	return m_DataSize;
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

CubeTextureData::CubeTextureData() : TextureData(nullptr, 0, false), m_Size(1)
{
}

CubeTextureData::CubeTextureData(uint32_t size): TextureData(nullptr, 0, false), m_Size(size)
{
}

CubeTextureData::CubeTextureData(uint32_t size, uint8_t* data, uint32_t dataSize, bool bTakeOwnership): TextureData(data, dataSize, bTakeOwnership), m_Size(size)
{
}

CubeTextureData& CubeTextureData::operator=(const CubeTextureData& data)
{
	TextureData::operator=(data);

	m_Size = data.m_Size;

	return *this;
}

CubeTextureData& CubeTextureData::operator=(CubeTextureData&& data)
{
	TextureData::operator=(std::move(data));

	m_Size = data.m_Size;
	data.m_Size = 0;

	return *this;
}

void CubeTextureData::SetSize(uint32_t size)
{
	m_Size = size;
}

uint32_t CubeTextureData::GetSize() const
{
	return m_Size;
}

void CubeTextureData::Serialize(Archive& archive)
{
	TextureData::Serialize(archive);

    archive & m_Size;
}

Texture2DArrayData::Texture2DArrayData() : TextureData(nullptr, 0, false), m_Width(1), m_Height(1), m_Depth(1)
{
}

Texture2DArrayData::Texture2DArrayData(int32_t width, int32_t height, int32_t depth) : TextureData(nullptr, 0, false), m_Width(width), m_Height(height), m_Depth(depth)
{
}

Texture2DArrayData::Texture2DArrayData(int32_t width, int32_t height, int32_t depth, uint8_t* data, uint32_t size, bool bTakeOwnership) : TextureData(data, size, bTakeOwnership), m_Width(width), m_Height(height), m_Depth(depth)
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

void Texture2DArrayData::SetWidth(uint32_t width)
{
	m_Width = width;
}

uint32_t Texture2DArrayData::GetWidth() const
{
	return m_Width;
}

void Texture2DArrayData::SetHeight(uint32_t height)
{
	m_Height = height;
}

uint32_t Texture2DArrayData::GetHeight() const
{
	return m_Height;
}

void Texture2DArrayData::SetDepth(uint32_t depth)
{
	m_Depth = depth;
}

uint32_t Texture2DArrayData::GetDepth() const
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

void Texture2DArrayData::Serialize(Archive& archive)
{
	TextureData::Serialize(archive);

    archive & m_Width;
    archive & m_Height;
    archive & m_Depth;
}
