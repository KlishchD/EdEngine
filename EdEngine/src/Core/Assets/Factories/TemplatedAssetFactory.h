#pragma once

#include "AssetFactory.h"
#include "Core/Assets/AssetManager.h"

template<typename T, AssetType m_Type>
class TemplatedAssetFactory : public AssetFactory
{
public:
	TemplatedAssetFactory(std::shared_ptr<AssetManager> manager);

	virtual std::shared_ptr<Asset> Create();
	virtual std::shared_ptr<Asset> Create(Archive& archive);
	virtual std::shared_ptr<Asset> Load(Archive& archive, bool bShouldLoadData);
	virtual AssetType GetType();
};

template<typename T, AssetType m_Type>
std::shared_ptr<Asset> TemplatedAssetFactory<T, m_Type>::Create()
{
	std::shared_ptr<T> asset = std::make_shared<T>();

	m_Manager->RegisterAsset(asset);

	return std::static_pointer_cast<Asset>(asset);
}

template<typename T, AssetType m_Type>
std::shared_ptr<Asset> TemplatedAssetFactory<T, m_Type>::Create(Archive& archive)
{
	std::shared_ptr<Asset> asset = Create();

	asset->Serialize(archive);

	return asset;
}

template<typename T, AssetType m_Type>
AssetType TemplatedAssetFactory<T, m_Type>::GetType()
{
	return m_Type;
}

template<typename T, AssetType m_Type>
std::shared_ptr<Asset> TemplatedAssetFactory<T, m_Type>::Load(Archive& archive, bool bShouldLoadData)
{
	std::shared_ptr<T> asset = std::make_shared<T>();
	asset->SetShouldLoadData(bShouldLoadData);
	asset->Serialize(archive);

	m_Manager->RegisterAsset(asset, archive.GetPath());

	return std::static_pointer_cast<Asset>(asset);
}

template<typename T, AssetType m_Type>
TemplatedAssetFactory<T, m_Type>::TemplatedAssetFactory(std::shared_ptr<AssetManager> manager) : AssetFactory(manager)
{	
}
