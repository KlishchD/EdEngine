#include "EdEngine.h"
#include "Asset.h"
#include "Helpers/AssetHelper.h"

Asset::Asset() : Data(nullptr), Id(AssetHelper::GenerateAssetID()), DataClaims(0), AssetType(AssetHelper::InvalidAssetTypeId), HasData(false), IsDirty(false), LoadData(false)
{

}

bool Asset::ShouldSerializeData(AssetArchive& archive) const
{
    bool attemptingToWriteData = archive.GetMode() == SerializationMode::Write && HasData && IsDirty;
    bool attemptingToReadData = archive.GetMode() == SerializationMode::Read && LoadData && (IsDirty || !HasData);
    return (attemptingToWriteData || attemptingToReadData);
}

void Asset::Serialize(AssetArchive& archive)
{
    archive & Name;
    archive & OriginPath;

    archive & Id;

    archive & AssetType;
}

void Asset::SerializeData(AssetArchive& archive)
{
    if (archive.GetMode() == SerializationMode::Read && !DataClaims)
    {
        FrameDataWasUnclaimed = Engine::Get().GetFrame();
    }
}

bool Asset::IsDataClaimed()
{
    return DataClaims;
}

void Asset::ClaimData()
{
//    ED_ASSERT(HasData, "Can not claim data if it is not loaded.");
    ++DataClaims;
}

void Asset::UnclaimData()
{
    ED_ASSERT(DataClaims, "Can not uncalim what was not claimed in a first place, probably there is a missmatch in claims and unclaims."); 
    --DataClaims;

    if (DataClaims == 0)
    {
        FrameDataWasUnclaimed = Engine::Get().GetFrame();
    }
}