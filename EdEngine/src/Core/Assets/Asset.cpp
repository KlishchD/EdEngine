#include "Asset.h"

#include "Descriptors/TextureDescriptor.h"
#include "Descriptors/MaterialDescriptor.h"
#include "Descriptors/StaticMeshDescriptor.h"
#include "Core/Macros.h"

void Asset::SetDescriptor(const std::shared_ptr<AssetDescriptor>& descriptor)
{
    m_Descriptor = descriptor;
}

void Asset::SyncDescriptor()
{
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

void Asset::MarkDirty()
{
    m_Descriptor->MarkDirty();
    // TODO: Maybe add define for editor only ?)
}

bool Asset::IsDirty() const
{
    return m_Descriptor->IsDirty();
}
