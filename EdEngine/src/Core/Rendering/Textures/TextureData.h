#pragma once
#include <cstdint>

struct TextureData
{
	virtual ~TextureData() = default;
};

struct Texture2DData : public TextureData
{
	Texture2DData() = default;
	Texture2DData(int32_t width, int32_t height, uint8_t* data);

	Texture2DData& operator=(Texture2DData&& data);

	virtual ~Texture2DData() override;

	int32_t Width = 1;
	int32_t Height = 1;
	uint8_t* Data = nullptr;
};

struct CubeTextureData : public TextureData
{
	CubeTextureData() = default;
	CubeTextureData(int32_t size, uint8_t* data[6]);
	CubeTextureData(CubeTextureData&& data) noexcept;

	CubeTextureData& operator=(CubeTextureData&&) = default;

	virtual ~CubeTextureData() override;

	int32_t Size;
	uint8_t* Data[6];
};
