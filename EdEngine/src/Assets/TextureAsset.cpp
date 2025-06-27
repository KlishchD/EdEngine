#include "EdEngine.h"
#include "TextureAsset.h"
#include "Helpers/AssetHelper.h"
#include "RenderingContext.h"

TextureAsset::TextureAsset() : BaseAsset(nullptr), Data(nullptr), Width(1), Height(1), Mips(1), RenderTextureType(TextureType::Texture2D), Format(PixelFormat::RGBA8F)
{

}

void TextureAsset::Serialize(AssetArchive& archive)
{
    archive & Width;
    archive & Height;
    archive & Mips;

    archive & RenderTextureType;
    archive & Format;
    archive & Meta;
}

bool TextureAsset::ShouldSerializeData(AssetArchive& archive) const
{
    return BaseAsset->ShouldSerializeData(archive);
}

void TextureAsset::SerializeData(AssetArchive& archive)
{
    archive.Serialize<u8, false>(&Data, GetDataSize());
    
    if (archive.GetMode() == SerializationMode::Read)
    {
        InitializeRenderTexture();
    }

    BaseAsset->HasData = true;
}

bool TextureAsset::InitializeRenderTexture()
{
    if (!Data || TextureView) return false;


    if (Mips > 1)
    {
        RenderingContext::Get().UploadTextureDeferred(Data, Width, Height, Mips, Format, TextureView, ResourceState::Common, BaseAsset->Name.c_str());
        Renderer::Get().RequestMipMapping(TextureView, static_cast<MipMappingMode>(Meta), Mips);
    }
    else
    {
        RenderingContext::Get().UploadTextureDeferred(Data, Width, Height, Mips, Format, TextureView, ResourceState::ShaderRead, BaseAsset->Name.c_str());
    }

    Memory::Get().AddDeallocation(&AssetDataAllocator(), &Data, GetDataSize(), MakeResourceUploadedPredicate(&TextureView));

    return true;
}

void TextureAsset::FreeData()
{
    ED_ASSERT(BaseAsset->HasData, "Called free data on an asset without data.");
    ED_ASSERT(!BaseAsset->IsDataClaimed(), "Called free data on an asset with claimed data.");

    RenderingContext::Get().FreeTexture(TextureView);

    BaseAsset->HasData = false;
}

TextureAsset::~TextureAsset()
{
    if (BaseAsset->HasData)
    {
        FreeData();
    }
}
