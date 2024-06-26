#include "AssetImporter.h"
#include "Helpers/FilesHelper.h"
#include "Core/Assets/AssetManager.h"

AssetImporter::AssetImporter(std::shared_ptr<AssetManager> manager) : m_Manager(manager)
{

}

AssetTypeImporter::AssetTypeImporter(std::shared_ptr<AssetManager> manager) : m_Manager(manager)
{

}

std::shared_ptr<Asset> AssetTypeImporter::Import(AssetType type, std::shared_ptr<AssetImportParameters> parameters)
{
	std::string path = FilesHelper::GetSavePath(parameters->Path, type);
	std::shared_ptr<Asset> asset = m_Manager->LoadAsset(path);
	return asset ? asset : m_Importers[type]->Import(parameters);
}

std::vector<std::shared_ptr<Asset>> AssetTypeImporter::ImportMultiple(AssetType type, std::shared_ptr<AssetImportParameters> parameters)
{
	std::vector<std::shared_ptr<Asset>> assets;

	for (const auto& [id, asset] : m_Manager->GetAssets())
	{
		if (asset->GetImportParameters()->Path == parameters->Path)
		{
			assets.push_back(asset);
		}
	}

	if (assets.empty())
	{
		assets = m_Importers[type]->ImportMultiple(parameters);
	}

	return assets;
}
