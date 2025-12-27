#pragma once

struct Asset;

struct TextureAsset
{
    enum MetaData : u8
    {
        Albedo = 0,
        Normal,
        Material,
        Count
    };

    static inline constexpr i32 Type = 3;

    Asset* BaseAsset;

    ResourceView TextureView;
  
    u8* Data;

    u32 Width;
    u32 Height;
    u32 Mips;

    TextureType RenderTextureType;
    PixelFormat Format;
    MetaData Meta;

    explicit TextureAsset();

    void Serialize(AssetArchive& archive);

    bool ShouldSerializeData(AssetArchive& archive) const;
    void SerializeData(AssetArchive& archive);
    bool InitializeRenderTexture();

    // Note: we do not store mips in Data and so they are not accounted here.
    u32 GetDataSize() const { return Width * Height * RenderTypes::GetPixelSize(Format); }

    void FreeData();

    ~TextureAsset();
};