#include "EdEngine.h"
#include "AssetManager.h"
#include "Helpers/AssetHelper.h"
#include "Helpers/FilesHelper.h"

AssetManager::AssetManager() : m_Assets("AssetsBase"), m_StaticMeshes("StaticMeshAssets"), m_Materials("MaterialsAssets"), m_Textures("TextureAssets"), m_Prefabs("PrefabAssets")
{
    ED_ASSERT(!s_Manager, "Only one asset manager can exist at once.");
    s_Manager = this;
}

AssetManager::~AssetManager()
{
    s_Manager = nullptr;
}

void AssetManager::Initialize()
{
    ED_LOG(AssetManager, info, "Initializing.");

    TemporaryArray<Path> paths = Files::GetContentPath().GetAllPathsTemporary(true);
    for (const Path& path : paths)
    {
        if (path.IsDirectory()) continue;

        u8 type = AssetHelper::GetAssetTypeFromExtension(path.GetExtension());

        if (type != AssetHelper::InvalidAssetTypeId)
        {
            Asset* asset = CreateAsset(type, "None", path, "None");
            asset->FilePath = path;
            LoadAsset(asset, false);
        }
    }
}

void AssetManager::Deinitialize()
{
    ED_LOG(AssetManager, info, "Deinitializing.");

    for (Asset* asset : m_Assets)
    {
        SaveAsset(asset);
    }
}

void AssetManager::Update(f32 deltaSeconds)
{
    // TODO: Make it more clever, we do not need to iterate all of the assets but some of them
    // and not all of them at one frame.

    const u32 frame = Engine::Get().GetFrame();

    constexpr u32 arbitraryFrameRangeSafeForAssetUnloading = 256;
    for (Asset* asset : m_Assets)
    {
        const bool hasFreeableData = asset->HasData && !asset->IsDirty;
        const bool isAllowedToFree = !asset->IsDataClaimed() && asset->FrameDataWasUnclaimed + arbitraryFrameRangeSafeForAssetUnloading < frame;
        if (hasFreeableData && isAllowedToFree)
        {
            UnloadAsset(asset);
        }
    }
}

Asset* AssetManager::CreateAsset(u8 type, ccstr8 name, const ContentPath& path, const Path& origin)
{
    switch (type)
    {
    case StaticMeshAsset::Type: return CreateStaticMesh(name, path, origin)->BaseAsset;
    case TextureAsset::Type:    return CreateTexture(name, path, origin)->BaseAsset;
    case MaterialAsset::Type:   return CreateMaterial(name, path, origin)->BaseAsset;
    case PrefabAsset::Type:     return CreatePrefab(name, path, origin)->BaseAsset;
    default:
        ED_ASSERT(false, "Requested asset type is not supported");
    }

    return nullptr;
}

template <typename AssetType, u32 PoolCount>
AssetType* CreateAssetInternal(PoolAllocator<Asset, AssetManager::MaxAssetsCount>& baseAllocator, PoolAllocator<AssetType, PoolCount>& typeAllocator, ccstr8 name, const ContentPath& path, const Path& origin)
{
    Asset* base = baseAllocator.Allocate();
    AssetType* asset = typeAllocator.Allocate();

    asset->BaseAsset = base;
    base->Data = static_cast<void*>(asset);
    base->AssetType = AssetType::Type;
    base->Name = name;
    base->OriginPath = origin;
    base->FilePath = path;
    base->IsDirty = true;

    return asset;
}

StaticMeshAsset* AssetManager::CreateStaticMesh(ccstr8 name, const ContentPath& path, const Path& origin)
{
    return CreateAssetInternal(m_Assets, m_StaticMeshes, name, path, origin);
}

MaterialAsset* AssetManager::CreateMaterial(ccstr8 name, const ContentPath& path, const Path& origin)
{
    return CreateAssetInternal(m_Assets, m_Materials, name, path, origin);
}

TextureAsset* AssetManager::CreateTexture(ccstr8 name, const ContentPath& path, const Path& origin)
{
    return CreateAssetInternal(m_Assets, m_Textures, name, path, origin);
}

PrefabAsset* AssetManager::CreatePrefab(ccstr8 name, const ContentPath& path, const Path& origin)
{
    return CreateAssetInternal(m_Assets, m_Prefabs, name, path, origin);
}

Asset* AssetManager::LoadAsset(Asset* asset, bool full)
{
    ED_ASSERT(asset, "Can not load nullptr assset.");

    if (full)
    {
        asset->ClaimData();
    }

    if (asset->HasData)
    {
        return asset;
    }

    ED_LOG(AssetManager, info, "Loading asset path [{}] full load [{}].", asset->FilePath.Get(), full);

    asset->LoadData = full;

    AssetArchive archive(asset->FilePath, SerializationMode::Read);

    archive & *asset;

    switch (asset->AssetType)
    {
    case StaticMeshAsset::Type:  archive & *static_cast<StaticMeshAsset*>(asset->Data); break;
    case TextureAsset::Type:     archive & *static_cast<TextureAsset*>(asset->Data); break;
    case MaterialAsset::Type:    archive & *static_cast<MaterialAsset*>(asset->Data); break;
    case PrefabAsset::Type:      archive & *static_cast<PrefabAsset*>(asset->Data); break;
    default:
        ED_ASSERT(false, "Load for requested asset type is not supported.");
    }

    asset->LoadData = false;
    asset->IsDirty = false;

    ED_LOG(AssetManager, info, "Loaded asset [{}] name [{}].", asset->Id, asset->Name);

    return asset;
}

Asset* AssetManager::LoadAsset(const ContentPath& path)
{
    for (Asset* asset : m_Assets)
    {
        if (asset->FilePath == path)
        {
            return LoadAsset(asset);
        }
    }

    ED_LOG(AssetManager, info, "Was not able to find requested asset to load.");

    return nullptr;
}

Asset* AssetManager::LoadAsset(u64 id)
{
    for (Asset* asset : m_Assets)
    {
        if (asset->Id == id)
        {
            return LoadAsset(asset);
        }
    }

    return nullptr;
}

void AssetManager::UnloadAsset(Asset* asset)
{
    ED_LOG(AssetManager, info, "Unloading asset name [{}] path [{}].", asset->Name, std::string(asset->FilePath.Get()));

    switch (asset->AssetType)
    {
    case StaticMeshAsset::Type:  static_cast<StaticMeshAsset*>(asset->Data)->FreeData(); break;
    case TextureAsset::Type:     static_cast<TextureAsset*>(asset->Data)->FreeData(); break;
    case MaterialAsset::Type:    static_cast<MaterialAsset*>(asset->Data)->FreeData(); break;
    case PrefabAsset::Type:      static_cast<PrefabAsset*>(asset->Data)->FreeData(); break;
    default:
        ED_ASSERT(false, "Load for requested asset type is not supported.");
    }
}

void AssetManager::SaveAsset(Asset* asset)
{
    if (asset->IsDirty)
    {
        ED_LOG(AssetManager, info, "Saving asset [{}] [{}] [{}] [{}]", asset->Id, asset->Name, asset->FilePath.Get(), asset->OriginPath.Get());

        AssetArchive archive(asset->FilePath, SerializationMode::Write);
        archive & *asset;

        switch (asset->AssetType)
        {
        case StaticMeshAsset::Type:  archive & *static_cast<StaticMeshAsset*>(asset->Data); break;
        case TextureAsset::Type:     archive & *static_cast<TextureAsset*>(asset->Data); break;
        case MaterialAsset::Type:    archive & *static_cast<MaterialAsset*>(asset->Data); break;
        case PrefabAsset::Type:      archive & *static_cast<PrefabAsset*>(asset->Data); break;
        default:
            ED_ASSERT(false, "Save for requested asset type is not supported");
        }

        asset->IsDirty = false;
    }
}

Asset* AssetManager::GetAsset(const ContentPath& path) const
{
    for (Asset* asset : m_Assets)
    {
        if (asset->FilePath.Equal(path))
        {
            return asset;
        }
    }

    return nullptr;
}

Asset* AssetManager::FindAsset(u64 id) const
{
    for (Asset* asset : m_Assets)
    {
        if (asset->Id == id)
        {
            return asset;
        }
    }

    return nullptr;
}
