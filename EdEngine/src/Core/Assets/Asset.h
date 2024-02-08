#pragma once

#include "Descriptors/AssetDescriptor.h"

class Asset
{
public:
    virtual void SetDescriptor(const std::shared_ptr<AssetDescriptor>& descriptor);

    virtual void SyncDescriptor();

    template <class T>
    std::shared_ptr<T> GetDescriptor() const;

    std::shared_ptr<AssetDescriptor> GetDescriptor() const;

    std::string GetAssetName() const;

    void MarkDirty();
    bool IsDirty() const;
protected:
    std::shared_ptr<AssetDescriptor> m_Descriptor;
};

template <class T>
std::shared_ptr<T> Asset::GetDescriptor() const
{
    return std::static_pointer_cast<T>(m_Descriptor);
}