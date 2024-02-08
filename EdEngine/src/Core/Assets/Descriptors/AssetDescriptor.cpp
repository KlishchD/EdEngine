#include "AssetDescriptor.h"
#include "Core/Macros.h"

void AssetDescriptor::SetShouldHaveData(bool status)
{
    m_bShouldHaveData = status;
}

bool AssetDescriptor::ShouldHaveData() const
{
    return m_bShouldHaveData;
}

bool AssetDescriptor::IsDirty() const
{
    return m_bIsDirty;
}

void AssetDescriptor::MarkDirty()
{
    m_bIsDirty = true;
}

bool AssetDescriptor::HasData() const
{
    return false;
}

void AssetDescriptor::ClaimData()
{
    ++m_DataClaims;
}

void AssetDescriptor::UnclaimData()
{
    ED_ASSERT(m_DataClaims, "Data was not claimed to be unclaimed")
    --m_DataClaims;
}
