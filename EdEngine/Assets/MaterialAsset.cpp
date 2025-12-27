#include "EdEngine.h"
#include "MaterialAsset.h"
#include "Helpers/AssetHelper.h"

void MaterialAsset::Serialize(AssetArchive& archive)
{
    AssetManager::Get().SerializeAsset(archive, &AlbedoTexture, false);
    AssetManager::Get().SerializeAsset(archive, &NormalTexture, false);
    AssetManager::Get().SerializeAsset(archive, &RoughnessTexture, false);
    AssetManager::Get().SerializeAsset(archive, &MetalicTexture, false);

    archive & BaseColor;

    archive & Roughness;
    archive & Metalic;
    archive & Emission;
}

bool MaterialAsset::ShouldSerializeData(AssetArchive& archive) const
{
    return BaseAsset->ShouldSerializeData(archive);
}

void MaterialAsset::SerializeData(AssetArchive& archive)
{
    if (archive.GetMode() == SerializationMode::Read)
    {
        LoadDependencies();
    }

    BaseAsset->HasData = true;
}

void MaterialAsset::LoadDependencies()
{
    if (AlbedoTexture)
    {
        AssetManager::Get().LoadAsset(AlbedoTexture->BaseAsset);
    }

    if (NormalTexture)
    {
        AssetManager::Get().LoadAsset(NormalTexture->BaseAsset);
    }
    
    if (RoughnessTexture)
    {
        AssetManager::Get().LoadAsset(RoughnessTexture->BaseAsset);
    }
    
    if (MetalicTexture)
    {
        AssetManager::Get().LoadAsset(MetalicTexture->BaseAsset);
    }
}

void MaterialAsset::FreeData()
{
    ED_ASSERT(BaseAsset->HasData, "Called free data on an asset without data.");
    ED_ASSERT(!BaseAsset->IsDataClaimed(), "Called free data on an asset with claimed data.");

    if (AlbedoTexture)
    {
        AlbedoTexture->BaseAsset->UnclaimData();
        AlbedoTexture = nullptr;
    }

    if (NormalTexture)
    {
        NormalTexture->BaseAsset->UnclaimData();
        NormalTexture = nullptr;
    }

    if (RoughnessTexture)
    {
        RoughnessTexture->BaseAsset->UnclaimData();
        RoughnessTexture = nullptr;
    }

    if (MetalicTexture)
    {
        MetalicTexture->BaseAsset->UnclaimData();
        MetalicTexture = nullptr;
    }

    BaseAsset->HasData = false;
}

MaterialAsset::~MaterialAsset()
{
    if (BaseAsset->HasData)
    {
        FreeData();
    }
}
