#pragma once

#include <filesystem>

#include "Core/Ed.h"

#include "Asset.h"
#include "StaticMesh.h"
#include "Core/BaseManager.h"
#include "Core/Math/Transform.h"

#include "Factories/AssetFactory.h"
#include "Importers/AssetImporter.h"

class Texture2DImportParameters;
class StaticMeshImportParameters;

class StaticMeshImportParameters;
class VertexArray;
class Shader;
class Scene;
class Texture2D;
class Material;

ED_CLASS(AssetManager) : public BaseManager
{
    ED_CLASS_BODY(AssetManager, BaseManager)
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;
    
    std::shared_ptr<Scene> CreateScene(const std::string& path);
    std::shared_ptr<Scene> LoadScene(const std::string& path);
   
    template <typename T> requires(std::is_base_of_v<Asset, T>)
    void RegisterAsset(std::shared_ptr<T> asset, const std::string& path = "");
    void RegisterAsset(std::shared_ptr<Asset> asset, const std::string& path = "");

    template<typename T> requires(std::is_base_of_v<Asset, T>)
    std::vector<std::shared_ptr<T>> GetAssets(AssetType type) const;

	template<typename T> requires(std::is_base_of_v<Asset, T>)
	std::shared_ptr<T> GetAsset(const std::string& path) const;
	std::shared_ptr<Asset> GetAsset(const std::string& path) const;

    template<typename T> requires(std::is_base_of_v<Asset, T>)
    std::shared_ptr<T> GetAsset(uint64_t id) const;
    std::shared_ptr<Asset> GetAsset(uint64_t id) const;

    template <typename T> requires(std::is_base_of_v<Asset, T>)
    std::shared_ptr<T> LoadAsset(uint64_t id) const;
	std::shared_ptr<Asset> LoadAsset(uint64_t id) const;

	template <typename T> requires(std::is_base_of_v<Asset, T>)
	std::shared_ptr<T> LoadAsset(const std::string& path) const;
	std::shared_ptr<Asset> LoadAsset(const std::string& path) const;

    template<typename T, typename E> requires(std::is_base_of_v<Asset, T> && std::is_base_of_v<AssetImportParameters, E>)
    std::shared_ptr<T> ImportAsset(AssetType type, std::shared_ptr<E> paramters);

	template<typename T> requires(std::is_base_of_v<Asset, T>)
    std::shared_ptr<T> CreateAsset(AssetType type, const std::string& path);

	const std::map<uint64_t, std::shared_ptr<Asset>>& GetAssets() const;

    AssetTypeFactory& GetFactory();
    AssetTypeImporter& GetImporter();
private:
    AssetTypeFactory m_Factory;
    AssetTypeImporter m_Importer;

    std::map<uint64_t, std::shared_ptr<Asset>> m_Assets;
    std::map<std::string, std::shared_ptr<Asset>> m_PathToAsset;

    std::map<std::string, std::shared_ptr<Scene>> m_Scenes;
};

template <typename T> requires(std::is_base_of_v<Asset, T>)
void AssetManager::RegisterAsset(std::shared_ptr<T> asset, const std::string& path)
{
	RegisterAsset(std::static_pointer_cast<Asset>(asset), path);
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::vector<std::shared_ptr<T>> AssetManager::GetAssets(AssetType type) const
{
    std::vector<std::shared_ptr<T>> assets;

    for (const auto& [id, asset] : m_Assets)
    {
        if (asset->GetType() == type)
        {
            assets.push_back(std::static_pointer_cast<T>(asset));            
        }
    }

    return assets;
}

template<typename T, typename E> requires(std::is_base_of_v<Asset, T> && std::is_base_of_v<AssetImportParameters, E>)
std::shared_ptr<T> AssetManager::ImportAsset(AssetType type, std::shared_ptr<E> paramters)
{
	return m_Importer.Import<T>(type, std::static_pointer_cast<AssetImportParameters>(paramters));
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<T> AssetManager::CreateAsset(AssetType type, const std::string& path)
{
	return m_Factory.Create<T>(type);
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<T> AssetManager::GetAsset(const std::string& path) const
{
	return std::static_pointer_cast<T>(GetAsset(path));
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<T> AssetManager::GetAsset(uint64_t id) const
{
    return std::static_pointer_cast<T>(GetAsset(id));
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<T> AssetManager::LoadAsset(uint64_t id) const
{
	return std::static_pointer_cast<T>(LoadAsset(id));
}

template<typename T> requires(std::is_base_of_v<Asset, T>)
std::shared_ptr<T> AssetManager::LoadAsset(const std::string& path) const
{
	return std::static_pointer_cast<T>(LoadAsset(path));
}
