#pragma once

#include "AssetDescriptor.h"
#include "Core/Assets/TextureImportParameters.h"
#include "Core/Rendering/Textures/TextureData.h"

struct TextureDescriptor : public AssetDescriptor
{
	virtual TextureImportParameters* GetImportParameters() const = 0;
	virtual TextureData* GetData() const = 0;
};

struct Texture2DDescriptor : public TextureDescriptor
{
	Texture2DImportParameters ImportParameters;
	Texture2DData Data;

	virtual TextureImportParameters* GetImportParameters() const override;
	virtual TextureData* GetData() const override;
};

struct CubeTextureDescriptor : public TextureDescriptor
{
	CubeTextureImportParameters ImportParameters;
	CubeTextureData Data;

	virtual TextureImportParameters* GetImportParameters() const override;
	virtual TextureData* GetData() const override;
};