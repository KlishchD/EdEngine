#pragma once

#include "AssetImporter.h"
#include "Core/Assets/ImportParameters/TextureImportParameters.h"

class Texture2DImporter : public AssetImporter
{
public:
	Texture2DImporter(std::shared_ptr<AssetManager> manager);

	virtual std::shared_ptr<Asset> Import(std::shared_ptr<AssetImportParameters> inParameters);
	virtual std::vector<std::shared_ptr<Asset>> ImportMultiple(std::shared_ptr<AssetImportParameters> parameters);
};
