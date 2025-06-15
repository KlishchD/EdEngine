#pragma once

struct Asset;

struct PrefabAsset
{
    static inline constexpr u32 Type = 4;

    Asset* BaseAsset;

    u32 EntitiesCount;
    u32 FeaturesCount;

    u32 BufferSize;
    u8* Buffer;

    void Serialize(AssetArchive& archive);

    bool ShouldSerializeData(AssetArchive& archive) const;
    void SerializeData(AssetArchive& archive);

    void FreeData();

    ~PrefabAsset();
};
