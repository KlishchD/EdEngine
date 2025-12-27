#pragma once

struct Asset
{
    Asset();

    std::string Name;
    Path OriginPath;
    ContentPath FilePath;

    void* Data;

    u64 Id;

    u16 AssetType;
    u16 FrameDataWasUnclaimed;
    u16 DataClaims;

    bool HasData;
    bool IsDirty;
    bool LoadData;

    template <typename T>
    T* GetData() const
    {
        return static_cast<T*>(Data);
    }

    bool ShouldSerializeData(AssetArchive& archive) const;
    void Serialize(AssetArchive& archive);
    void SerializeData(AssetArchive& archive);

    bool IsDataClaimed();
    void ClaimData();
    void UnclaimData();
};