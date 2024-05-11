#include "AssetManager.h"

#include "StaticMesh.h"
#include "Material.h"
#include "Core/Rendering/Textures/Texture.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Textures/CubeTexture.h"

#include "Platform/Rendering/OpenGL/Textures/OpenGLCubeTexture.h" // TODO: Fix for the API !!!
#include "Platform/Rendering/OpenGL/Textures/OpenGLTexture2D.h"

#include "Core/Scene.h"
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "Utils/AssetUtils.h"
#include "Utils/Files.h"

#include "Core/Macros.h"

#include "Factories/TemplatedAssetFactory.h"
#include "Importers/Texture2DAssetImporter.h"
#include "Importers/MaterialAssetImporter.h"
#include "Importers/StaticMeshImporter.h"

void AssetManager::Initialize(Engine* engine)
{
	ED_LOG(AssetManager, info, "Started initalizing")

    m_Importer = AssetTypeImporter(std::static_pointer_cast<AssetManager>(shared_from_this()));
    m_Importer.RegisterImporter<Texture2DImporter>(AssetType::Texture2D);
    m_Importer.RegisterImporter<MaterialAssetImporter>(AssetType::Material);
    m_Importer.RegisterImporter<StaticMeshImporter>(AssetType::StaticMesh);

    m_Factory = AssetTypeFactory(std::static_pointer_cast<AssetManager>(shared_from_this())); // TODO: Fix for the API !!!
    m_Factory.RegisterFactory<TemplatedAssetFactory<OpenGLCubeTexture, AssetType::CubeTexture>>(AssetType::CubeTexture);
    m_Factory.RegisterFactory<TemplatedAssetFactory<OpenGLTexture2D, AssetType::Texture2D>>(AssetType::Texture2D);
    m_Factory.RegisterFactory<TemplatedAssetFactory<Material, AssetType::Material>>(AssetType::Material);
    m_Factory.RegisterFactory<TemplatedAssetFactory<StaticMesh, AssetType::StaticMesh>>(AssetType::StaticMesh);

	std::filesystem::recursive_directory_iterator iterator(Files::ContentFolderPath);
	for (const std::filesystem::directory_entry& entry : iterator)
	{
		if (entry.is_directory()) continue;
		if (std::string extension = entry.path().extension().string(); AssetUtils::IsAssetExtension(extension))
		{
			std::string path = entry.path().string();

			ED_LOG(AssetManager, info, "Started loading asset {}", path)

            std::shared_ptr<Asset> asset = m_Factory.Load(path, false);
            RegisterAsset(asset, path);

			ED_LOG(AssetManager, info, "Finished loading asset {}", path)
		}
	}

    ED_LOG(AssetManager, info, "Finished initalizing")
}

void AssetManager::Deinitialize()
{
	ED_LOG(AssetManager, info, "Started deinitializing")

    for (const auto& [id, asset] : m_Assets)
    {
		const std::string& path = asset->GetImportParameters()->Path;

		ED_LOG(AssetManager, info, "Started saving scene: {}", path)

		if (asset->IsDirty())
		{
			Archive archive(path, ArchiveMode::Write);
			asset->Serialize(archive);
			asset->SerializeData(archive);
		}

		ED_LOG(AssetManager, info, "Finished saving scene: {}", path)
    }

    for (const auto& [path, scene]: m_Scenes)
    {
		ED_LOG(AssetManager, info, "Started saving scene: {}", path)

		Archive archive(path, ArchiveMode::Write);
		scene->Serialize(archive);

		ED_LOG(AssetManager, info, "Finished saving scene: {}", path)
    }

	ED_LOG(AssetManager, info, "Finished deinitializing")
}

std::shared_ptr<Scene> AssetManager::CreateScene(const std::string& path)
{
    ED_LOG(AssetManager, info, "Started creating scene: {}", path)

	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	m_Scenes[path] = scene;
	
    Archive archive(path, ArchiveMode::Write);
    scene->Serialize(archive);

    ED_LOG(AssetManager, info, "Finished creating scene: {}", path)
	
    return scene;
}

std::shared_ptr<Scene> AssetManager::LoadScene(const std::string& path)
{
    ED_LOG(AssetManager, info, "Started loading scene: {}", path)
 
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();

    std::filesystem::directory_entry entry(path);

    if (!entry.exists() || entry.is_directory())
    {
        ED_LOG(AssetManager, info, "Couldn't find the scene: {}", path)
        return nullptr;
    }

	Archive archive(path, ArchiveMode::Read);
    scene->Serialize(archive);
    
    m_Scenes[path] = scene;
    
    ED_LOG(AssetManager, info, "Finished loading scene: {}", path)
    
    return scene;
}

AssetTypeFactory& AssetManager::GetFactory()
{
    return m_Factory;
}

AssetTypeImporter& AssetManager::GetImporter()
{
    return m_Importer;
}

void AssetManager::RegisterAsset(std::shared_ptr<Asset> asset, const std::string& path)
{
    m_Assets[asset->GetId()] = asset;

    ED_ASSERT_CONTEXT(AssetManager, !path.empty(), "Path cannot be empty")
	m_PathToAsset[path] = asset;
}

std::shared_ptr<Asset> AssetManager::LoadAsset(const std::string& path) const
{
	if (!m_PathToAsset.count(path))
	{
		return nullptr;
	}

	std::shared_ptr<Asset> asset = m_PathToAsset.at(path);

	if (!asset->HasData())
	{
		Archive archive(path, ArchiveMode::Read);
		asset->Serialize(archive);
		asset->SerializeData(archive);
	}

	return asset;
}

std::shared_ptr<Asset> AssetManager::LoadAsset(UUID id) const
{
    if (!m_Assets.count(id))
    {
        return nullptr;
    }

    std::shared_ptr<Asset> asset = m_Assets.at(id);

    if (!asset->HasData())
    {
        Archive archive(asset->GetImportParameters()->Path, ArchiveMode::Read);
        asset->Serialize(archive);
        asset->SerializeData(archive);
    }

    return asset;
}

const std::map<UUID, std::shared_ptr<Asset>>& AssetManager::GetAssets() const
{
    return m_Assets;
}

std::shared_ptr<Asset> AssetManager::GetAsset(UUID id) const
{
    return m_Assets.count(id) ? m_Assets.at(id) : nullptr;
}

std::shared_ptr<Asset> AssetManager::GetAsset(const std::string& path) const
{
    return m_PathToAsset.count(path) ? m_PathToAsset.at(path) : nullptr;
}
