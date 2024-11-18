#pragma once

#include "Core/Assets/Asset.h"

class AssetManager;

class AssetFactory
{
public:
	AssetFactory(std::shared_ptr<AssetManager> manager);

	virtual std::shared_ptr<Asset> Create() = 0;
	virtual std::shared_ptr<Asset> Create(Archive& archive) = 0;
	virtual AssetType GetType();

	virtual ~AssetFactory() = default;

protected:
	std::shared_ptr<AssetManager> m_Manager;
};

class AssetTypeFactory
{
public:
	AssetTypeFactory(std::shared_ptr<AssetManager> manager = nullptr);

	template<typename FactoryType>
	void RegisterFactory(AssetType type)
	{
		m_Factories[type] = std::make_shared<FactoryType>(m_Manager);
	}

	template<typename T> requires(std::is_base_of_v<Asset, T>)
	std::shared_ptr<T> Create(AssetType type);
	std::shared_ptr<Asset> Create(AssetType type);

	template<typename T> requires(std::is_base_of_v<Asset, T>)
	std::shared_ptr<T> Create(AssetType type, const std::string& path);
	std::shared_ptr<Asset> Create(AssetType type, const std::string& path);

	template<typename T> requires(std::is_base_of_v<Asset, T>)
	std::shared_ptr<T> Load(Archive& arcive, bool bShouldLoadData);
	std::shared_ptr<Asset> Load(Archive& arcive, bool bShouldLoadData);

	template<typename T> requires(std::is_base_of_v<Asset, T>)
	std::shared_ptr<Asset> Load(const std::string& path, bool bShouldLoadData);
	std::shared_ptr<Asset> Load(const std::string& path, bool bShouldLoadData);

	virtual ~AssetTypeFactory() = default;
protected:
	std::shared_ptr<AssetManager> m_Manager;
	std::unordered_map<AssetType, std::shared_ptr<AssetFactory>> m_Factories;
};

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<T> AssetTypeFactory::Create(AssetType type)
{
	return std::static_pointer_cast<T>(Create(type));
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<T> AssetTypeFactory::Create(AssetType type, const std::string& path)
{
	return std::static_pointer_cast<T>(Create(type, path));
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<T> AssetTypeFactory::Load(Archive& arcive, bool bShouldLoadData)
{
	return std::static_pointer_cast<T>(Load(arcive, bShouldLoadData));
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<Asset> AssetTypeFactory::Load(const std::string& path, bool bShouldLoadData)
{
	return std::static_pointer_cast<T>(Load(path, bShouldLoadData));
}
