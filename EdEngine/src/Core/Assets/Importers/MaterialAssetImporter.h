#pragma once

#include "AssetImporter.h"
#include "Core/Assets/Material.h"
#include <assimp/Importer.hpp>
#include "Core/Assets/ImportParameters/MaterialImportParameters.h"

struct aiMaterial;
class Texture2D;

class MaterialAssetImporter : public AssetImporter
{
public:
	MaterialAssetImporter(std::shared_ptr<AssetManager> manager);

	virtual std::shared_ptr<Asset> Import(std::shared_ptr<AssetImportParameters> parameters);
	virtual std::vector<std::shared_ptr<Asset>> ImportMultiple(std::shared_ptr<AssetImportParameters> parameters);

protected:
	std::shared_ptr<Material> ImportMaterial(const aiMaterial* inMaterial, std::shared_ptr<AssetImportParameters> inParameters);
	std::shared_ptr<Material> ParseMaterial(const aiMaterial* inMaterial, const std::string& materialPath);

protected:
	Assimp::Importer m_Importer;
};
