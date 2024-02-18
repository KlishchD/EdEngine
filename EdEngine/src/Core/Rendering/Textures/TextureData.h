#pragma once

#include "Utils/SerializationUtils.h"

class TextureData
{
public:
	BOOST_SERIALIZATION_SPLIT_MEMBER()

	TextureData();
	TextureData(void* data, uint32_t size, bool bTakeOwnership);

	TextureData& operator=(const TextureData& data);
	TextureData& operator=(TextureData&& data);

	template <class Archive>
	void save(Archive& ar, uint32_t version) const
	{
		ar & m_DataSize;
		ar & boost::serialization::make_binary_object(m_Data, m_DataSize);
	}

	template <class Archive>
	void load(Archive& ar, uint32_t version)
	{
		ar & m_DataSize;

		m_Data = malloc(m_DataSize);
		ar & boost::serialization::make_binary_object(m_Data, m_DataSize);
	}

	void SetData(void* data, uint32_t size, bool bTakeOwnership);
	void* GetData() const;
	uint32_t GetDataSize() const;

	virtual ~TextureData();
protected:
	void FreeData();
private:
	uint32_t m_DataSize = 0;
	bool m_bDataOwner = false;
	void* m_Data = nullptr;
};

class Texture2DData : public TextureData
{
public:
	Texture2DData();
	Texture2DData(int32_t width, int32_t height);
	Texture2DData(int32_t width, int32_t height, void* data, uint32_t size, bool bTakeOwnership);

	Texture2DData& operator=(const Texture2DData& data);
	Texture2DData& operator=(Texture2DData&& data);

	void SetWidth(uint32_t width);
	uint32_t GetWidth() const;

	void SetHeight(uint32_t height);
	uint32_t GetHeight();

	void SetSize(glm::u32vec2 size);
	glm::u32vec2 GetSize() const;

	template <class Archive>
	void serialize(Archive& ar, uint32_t version)
	{
		ar & boost::serialization::base_object<TextureData>(*this);
		ar & m_Width;
		ar & m_Height;
	}
private:
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
};

class CubeTextureData : public TextureData
{
public:
	CubeTextureData();
	CubeTextureData(uint32_t size);
	CubeTextureData(uint32_t size, void* data, uint32_t dataSize, bool bTakeOwnership);

	CubeTextureData& operator=(const CubeTextureData& data);
	CubeTextureData& operator=(CubeTextureData&& data);

	void SetSize(uint32_t size);
	uint32_t GetSize() const;

	template <class Archive>
	void serialize(Archive& ar, uint32_t version)
	{
		ar & boost::serialization::base_object<TextureData>(*this);
		ar & m_Size;
	}
private:
	uint32_t m_Size = 0;
};

class Texture2DArrayData : public TextureData
{
public:
	Texture2DArrayData();
	Texture2DArrayData(int32_t width, int32_t height, int32_t depth);
	Texture2DArrayData(int32_t width, int32_t height, int32_t depth, uint8_t* data, uint32_t size, bool bTakeOwnership);

	Texture2DArrayData& operator=(const Texture2DArrayData& data);
	Texture2DArrayData& operator=(Texture2DArrayData&& data);

	void SetWidth(uint32_t width);
	uint32_t GetWidth() const;

	void SetHeight(uint32_t height);
	uint32_t GetHeight();

	void SetDepth(uint32_t depth);
	uint32_t GetDepth() const;

	void SetSize(glm::u32vec3 size);
	glm::u32vec3 GetSize() const;

	template <class Archive>
	void serialize(Archive& ar, uint32_t version)
	{
		ar & boost::serialization::base_object<TextureData>(*this);
		ar & m_Width;
		ar & m_Height;
		ar & m_Depth;
	}
private:
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	uint32_t m_Depth = 0;
};