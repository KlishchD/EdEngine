#pragma once

#include "AssetImportParameters.h"
#include "Core/Rendering/Types.h"

struct TextureImportParameters : public AssetImportParameters
{
	WrapMode WrapS = WrapMode::Repeat;
	WrapMode WrapT = WrapMode::Repeat;

	PixelFormat Format = PixelFormat::RGB8F;
	FilteringMode Filtering = FilteringMode::Linear;

	virtual void Serialize(Archive& archive) override;
};

struct Texture2DImportParameters : public TextureImportParameters
{
	bool GenerateMipMaps = false;

	virtual void Serialize(Archive& archive) override;
};

struct Texture2DArrayImportParameters : public TextureImportParameters
{

};

struct CubeTextureImportParameters: public TextureImportParameters
{
	WrapMode WrapR = WrapMode::Repeat;

	virtual void Serialize(Archive& archive) override;
};