#include "EdEngine.h"
#include "PrefabAsset.h"

void PrefabAsset::Serialize(AssetArchive& archive)
{
    archive & EntitiesCount;
    archive & FeaturesCount;

    archive & BufferSize;
}

bool PrefabAsset::ShouldSerializeData(AssetArchive& archive) const
{
    return BaseAsset->ShouldSerializeData(archive);
}

void PrefabAsset::SerializeData(AssetArchive& archive)
{
    archive.Serialize<u8, false>(&Buffer, BufferSize);
}

void PrefabAsset::FreeData()
{
    ED_ASSERT(BaseAsset->HasData, "Called free data on an asset without data.");
    ED_ASSERT(!BaseAsset->IsDataClaimed(), "Called free data on an asset with claimed data.");

    AssetDataAllocator().Deallocate(Buffer, BufferSize);

    BaseAsset->HasData = false;
}

PrefabAsset::~PrefabAsset()
{
    if (BaseAsset->HasData)
    {
        FreeData();
    }
}
