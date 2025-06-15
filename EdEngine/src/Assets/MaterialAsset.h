#pragma once

struct Asset;
struct TextureAsset;

struct MaterialAsset
{
    static inline constexpr i32 Type = 2;

    Asset* BaseAsset;

    // TODO: Serialize AssetID not AssetHandle ;)
    TextureAsset* AlbedoTexture;
    TextureAsset* NormalTexture;
    TextureAsset* RoughnessTexture;
    TextureAsset* MetalicTexture;

    glm::vec3 BaseColor = glm::vec3(1.0f);

    f32 Roughness = 0.5f;
    f32 Metalic = 0.5f;
    f32 Emission = 0.0f;

    void Serialize(AssetArchive& archive);

    bool ShouldSerializeData(AssetArchive& archive) const;
    void SerializeData(AssetArchive& archive);

    void LoadDependencies();

    void FreeData();

    ~MaterialAsset();
};