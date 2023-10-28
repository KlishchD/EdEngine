#include "Asset.h"

int32_t AssetDescriptor::GetAssetId() const
{
    return AssetId;
}

void Asset::SetDescriptor(const std::shared_ptr<AssetDescriptor>& descriptor)
{
    m_Descriptor = descriptor;
}

std::shared_ptr<AssetDescriptor> Asset::GetDescriptor() const
{
    return m_Descriptor;
}

std::string Asset::GetAssetName() const
{
    if (!m_Descriptor)
    {
        return "None";
    }
    return m_Descriptor->AssetName;
}
