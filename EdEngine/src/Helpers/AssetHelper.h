#pragma once

struct Asset;
struct aiMesh;
struct aiMaterial;
struct aiTexture;

struct StaticMeshAsset;
struct MaterialAsset;
struct TextureAsset;

struct ImportedScene
{
    PrefabAsset* Prefab;
    Array<TextureAsset*> Textures;
    Array<MaterialAsset*> Materials;
    Array<StaticMeshAsset*> StaticMeshes;
};

namespace AssetHelper
{
    struct TextureSpecification
    {
        Path Origin;
        ccstr8 Name;
        PixelFormat Format;
        TextureAsset::MetaData Meta;
        u32 MipsCount;
    };

    // TODO: At the moment this system seems to be better than enum
    constexpr u32 InvalidAssetTypeId = 0;

    u32 GetAssetTypeFromExtension(ccstr8 extension);
    bool IsAssetExtension(ccstr8 extension);

    ccstr8 GetSaveExtensions(u32 type);
    ContentPath GetSavePath(const ContentPath& save, u32 type, ccstr8 filename = "");

    ccstr8 GetAssetNameLable(const Asset& asset);

    u64 GenerateAssetID();

    ImportedScene ImportScene(const Path& origin, const ContentPath& save, ccstr8 prefabName, bool createPrefab, bool optimizeScene);

    TextureAsset* ImportOrLoadTexture(const Path& origin, const ContentPath& save, TextureAsset::MetaData meta, ccstr8 name, PixelFormat format, u32 mips);
    TextureAsset* ImportSingleTexture(const Path& origin, const ContentPath& save, TextureAsset::MetaData meta, ccstr8 name, PixelFormat format, u32 mips);
    TextureAsset* ImportSingleTexture(const ContentPath& save, const TextureSpecification& specification);
    
    Array<StaticMeshAsset*> ImportMultipleStaticMeshes(const Path& origin, const ContentPath& save, aiMesh** meshes, u32 count, const Transform& transform = Transform::Unit);
    Array<MaterialAsset*> ImportMultipleMaterials(const Path& origin, const ContentPath& save, aiMaterial** materials, u32 count, TemporaryArray<TextureSpecification>& textures);
    Array<TextureAsset*> ImportMultipleTextures(const ContentPath& save, const TemporaryArray<TextureSpecification>& textures);

    //Array<TextureAsset*> ImportMultipleTextures(const Path& origin, const ContentPath& save, aiTexture** textures, u32 count, bool mips);

    void MatchTextures(const Array<MaterialAsset*>& materials, const Array<TextureAsset*>& textures);
    void MatchMaterials(const Array<StaticMeshAsset*>& meshes, const Array<MaterialAsset*>& materials);

    void LoadTextures(const Array<TextureAsset*>& textures);
    void LoadMaterails(const Array<MaterialAsset*>& materials);
    void LoadStaticMeshes(const Array<StaticMeshAsset*>& meshes);

    PrefabAsset* CreatePrefabAsset(ccstr8 name, const ContentPath& save, const Array<Entity*>& entities);
    PrefabAsset* CreatePrefabAsset(Prefab* prefab, const ContentPath& save, const Path& origin);
};
