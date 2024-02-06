#pragma once

#include "Utils/SerializationUtils.h"

struct TextureData
{
	uint32_t PixelSize = 4 * sizeof(uint8_t);

	virtual ~TextureData() = default;
};

struct Texture2DData : public TextureData
{
	BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	int32_t Width = 1;
	int32_t Height = 1;
	uint8_t* Data = nullptr;
public:
	Texture2DData() = default;
	Texture2DData(int32_t width, int32_t height, uint8_t* data);

	Texture2DData& operator=(const Texture2DData& data);
	Texture2DData& operator=(Texture2DData&& data);

	virtual ~Texture2DData() override;

	template <class Archive>
	void save(Archive& ar, uint32_t version) const
	{
		ar & Width;
		ar & Height;

		int32_t size = Width * Height * PixelSize;
		ar & boost::serialization::make_binary_object(Data, size);
	}

	template <class Archive>
	void load(Archive& ar, uint32_t version)
	{
		ar & Width;
		ar & Height;

		int32_t size = Width * Height * PixelSize;
		Data = static_cast<uint8_t*>(std::malloc(size));
		ar & boost::serialization::make_binary_object(Data, size);
	}

};

struct CubeTextureData : public TextureData
{
	BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	int32_t Size;
	uint8_t* Data[6];
public:
	CubeTextureData() = default;
	CubeTextureData(int32_t size, uint8_t* data[6]);
	CubeTextureData(CubeTextureData&& data) noexcept;

	CubeTextureData& operator=(CubeTextureData&&) = default;

	virtual ~CubeTextureData() override;

	template <class Archive>
	void save(Archive& ar, uint32_t version) const
	{
		ar & Size;

		int32_t size = Size * Size * PixelSize;
		for (int32_t i = 0; i < 6; ++i)
		{
			ar & boost::serialization::make_binary_object(Data[i], size);
		}
	}

	template <class Archive>
	void load(Archive& ar, uint32_t version)
	{
		ar & Size;

		int32_t size = Size * Size * PixelSize;
		for (int32_t i = 0; i < 6; ++i)
		{
			Data[i] = static_cast<uint8_t*>(std::malloc(size));
			ar & boost::serialization::make_binary_object(Data[i], size);
		}
	}
};