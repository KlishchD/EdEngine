#pragma once

#include "Core/Assets/Asset.h"

class AssetManager;

// TODO: Force appropriate import parameters to asset

class AssetImporter
{
public:
	AssetImporter(std::shared_ptr<AssetManager> manager);

	virtual std::shared_ptr<Asset> Import(std::shared_ptr<AssetImportParameters> parameters) = 0;
	virtual std::vector<std::shared_ptr<Asset>> ImportMultiple(std::shared_ptr<AssetImportParameters> parameters) = 0;
protected:
	std::shared_ptr<AssetManager> m_Manager;
};

class AssetTypeImporter
{
public:
	AssetTypeImporter(std::shared_ptr<AssetManager> manager = nullptr);

	template<typename ImporterType>
	void RegisterImporter(AssetType type);

	template<typename T, typename E> requires (std::is_base_of_v<AssetImportParameters, E>)
	std::shared_ptr<T> Import(AssetType type, std::shared_ptr<E> parameters)
	{
		return std::static_pointer_cast<T>(Import(type, std::static_pointer_cast<AssetImportParameters>(parameters)));
	}

	template<typename T, typename E> requires (std::is_base_of_v<AssetImportParameters, E>)
	std::vector<std::shared_ptr<T>> ImportMultiple(AssetType type, std::shared_ptr<E> parameters)
	{
		std::vector<std::shared_ptr<Asset>> assets = ImportMultiple(type, std::static_pointer_cast<AssetImportParameters>(parameters));
		std::vector<std::shared_ptr<T>> castedAssets;

		for (const std::shared_ptr<Asset> asset : assets)
		{
			castedAssets.push_back(std::static_pointer_cast<T>(asset));
		}

		return castedAssets;
	}

	std::shared_ptr<Asset> Import(AssetType type, std::shared_ptr<AssetImportParameters> parameters);
	std::vector<std::shared_ptr<Asset>> ImportMultiple(AssetType type, std::shared_ptr<AssetImportParameters> parameters);

	virtual ~AssetTypeImporter() = default;
protected:
	std::shared_ptr<AssetManager> m_Manager;
	std::unordered_map<AssetType, std::shared_ptr<AssetImporter>> m_Importers;
};

template<typename ImporterType>
void AssetTypeImporter::RegisterImporter(AssetType type)
{
	m_Importers[type] = std::make_shared<ImporterType>(m_Manager);
}
