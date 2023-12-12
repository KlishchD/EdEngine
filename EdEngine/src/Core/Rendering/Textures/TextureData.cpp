#include "TextureData.h"

Texture2DData::Texture2DData(int32_t width, int32_t height, uint8_t* data) : Width(width), Height(height), Data(data)
{}

Texture2DData& Texture2DData::operator=(Texture2DData&& data)
{
	Width = data.Width;
	Height = data.Height;
	Data = data.Data;
	data.Data = nullptr;
	return *this;
}

Texture2DData::~Texture2DData()
{
	if (Data)
	{
		delete Data;
		Data = nullptr;
	}
}

CubeTextureData::CubeTextureData(int32_t size, uint8_t* data[6]): Size(size)
{
	for (int32_t i = 0; i < 6; ++i)
	{
		Data[i] = data[i];
	}
}

CubeTextureData::CubeTextureData(CubeTextureData&& data) noexcept : Size(data.Size)
{
	for (int32_t i = 0; i < 6; ++i)
	{
		Data[i] = data.Data[i];
		data.Data[i] = nullptr;
	}
}

CubeTextureData::~CubeTextureData()
{
	for (int32_t i = 0; i < 6; ++i)
	{
		if (Data[i])
		{
			delete Data[i];
			Data[i] = nullptr;
		}
	}
}
