#pragma once

#include "AssetImportParameters.h"
#include "Core/Rendering/Types.h"

ED_CLASS(TextureImportParameters) : public AssetImportParameters
{
	ED_CLASS_BODY(TextureImportParameters, AssetImportParameters)
public:
	WrapMode WrapS = WrapMode::Repeat;
	WrapMode WrapT = WrapMode::Repeat;

	PixelFormat Format = PixelFormat::RGB8F;
	FilteringMode Filtering = FilteringMode::Linear;

	virtual void Serialize(Archive& archive) override;
};

ED_CLASS(Texture2DImportParameters) : public TextureImportParameters
{
	ED_CLASS_BODY(Texture2DImportParameters, TextureImportParameters)
public:
	bool GenerateMipMaps = false;

	virtual void Serialize(Archive& archive) override;
};

ED_CLASS(Texture2DArrayImportParameters) : public TextureImportParameters
{
	ED_CLASS_BODY(Texture2DImportParameters, TextureImportParameters)
public:
};

ED_CLASS(CubeTextureImportParameters) : public TextureImportParameters
{
	ED_CLASS_BODY(CubeTextureImportParameters, TextureImportParameters)
public:
	WrapMode WrapR = WrapMode::Repeat;

	virtual void Serialize(Archive& archive) override;
};