#pragma once

#include <string>

#include "Core/Rendering/Types.h"

struct TextureImportParameters
{
	TextureImportParameters() = default;
	TextureImportParameters(const std::string& path, WrapMode wrapS, WrapMode wrapT, PixelFormat format, FilteringMode filtering);
	
	std::string Path;

	WrapMode WrapS = WrapMode::Repeat;
	WrapMode WrapT = WrapMode::Repeat;

	PixelFormat Format = PixelFormat::RGB8F;
	FilteringMode Filtering = FilteringMode::Linear;
};

struct Texture2DImportParameters : public TextureImportParameters
{
	Texture2DImportParameters() = default;
	Texture2DImportParameters(const std::string& path, WrapMode wrapS, WrapMode wrapT, PixelFormat format, FilteringMode filtering);
};

struct CubeTextureImportParameters: public TextureImportParameters
{
	CubeTextureImportParameters() = default;
	CubeTextureImportParameters(const std::string& path, WrapMode wrapS, WrapMode wrapT, WrapMode wrapR, PixelFormat format, FilteringMode filtering);

	WrapMode WrapR = WrapMode::Repeat;
};
