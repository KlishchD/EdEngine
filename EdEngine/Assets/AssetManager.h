#pragma once

struct Asset;
struct StaticMeshAsset;
struct MaterialAsset;
struct TextureAsset;
struct PrefabAsset;

class AssetManager
{
public:
  static constexpr u32 InvalidAssetHandle = -1;

  static constexpr u32 MaxStaticMeshesCount = 4 * 1024;
  static constexpr u32 MaxMaterialsCount = 4 * 1024;
  static constexpr u32 MaxTexturesCount = 4 * 1024;
  static constexpr u32 MaxPrefabsCount = 1024;
  static constexpr u32 MaxAssetsCount = MaxStaticMeshesCount + MaxMaterialsCount + MaxTexturesCount + MaxPrefabsCount;
public:
  static inline AssetManager& Get()
  {
    ED_ASSERT(s_Manager, "Manager was not initialized.");
    return *s_Manager;
  }

  void Initialize();
  void Deinitialize();
  void Update(f32 deltaSeconds);

  Asset* CreateAsset(u8 type, ccstr8 name, const ContentPath& path, const Path& origin);
  StaticMeshAsset* CreateStaticMesh(ccstr8 name, const ContentPath& path, const Path& origin);
  MaterialAsset* CreateMaterial(ccstr8 name, const ContentPath& path, const Path& origin);
  TextureAsset* CreateTexture(ccstr8 name, const ContentPath& path, const Path& origin);
  PrefabAsset* CreatePrefab(ccstr8 name, const ContentPath& path, const Path& origin);

  Asset* LoadAsset(Asset* asset, bool full = true);
  Asset* LoadAsset(const ContentPath& path);
  Asset* LoadAsset(u64 id);

  void UnloadAsset(Asset* asset);

  void SaveAsset(Asset* asset);

  Asset* GetAsset(const ContentPath& path) const;
  Asset* FindAsset(u64 id) const;

  template <typename T, FreeListAllocatorGetter Allocator>
  void SerializeAsset(Archive<Allocator>& archive, T** asset, bool loadData)
  {
    if (archive.GetMode() == SerializationMode::Read)
    {
      u64 id;
      archive& id;

      Asset* base = nullptr;
      if (loadData)
      {
        base = LoadAsset(id);
      }
      else
      {
        base = FindAsset(id);
      }

      if (base)
      {
        (*asset) = reinterpret_cast<T*>(base->Data);
      }
    }
    else
    {
      if (asset && *asset)
      {
        archive& (*asset)->BaseAsset->Id;
      }
      else
      {
        archive & 1;
      }
    }
  }

  const PoolAllocator<StaticMeshAsset, MaxStaticMeshesCount>& GetStaticMeshes() const { return m_StaticMeshes; }
  const PoolAllocator<MaterialAsset, MaxMaterialsCount>& GetMaterials() const { return m_Materials; }
  const PoolAllocator<TextureAsset, MaxTexturesCount>& GetTextures() const { return m_Textures; }
  const PoolAllocator<PrefabAsset, MaxPrefabsCount>& GetPrefabs() const { return m_Prefabs; }

protected:
  void automatic_unloading_update();

protected:
  friend class Engine;
  AssetManager();
  ~AssetManager();

protected:
  inline static AssetManager* s_Manager;

  PoolAllocator<Asset, MaxAssetsCount> m_Assets;
  PoolAllocator<StaticMeshAsset, MaxStaticMeshesCount> m_StaticMeshes;
  PoolAllocator<MaterialAsset, MaxMaterialsCount> m_Materials;
  PoolAllocator<TextureAsset, MaxTexturesCount> m_Textures;
  PoolAllocator<PrefabAsset, MaxPrefabsCount> m_Prefabs;
};