#pragma once

#include "Core/Ed.h"

enum class AssetType : uint8_t
{
    Texture2D,
    Texture3D,
    Material,
    StaticMesh
};

struct AssetDescriptor
{
    virtual ~AssetDescriptor() = default;

    int32_t AssetId;
    std::string AssetName;
    AssetType AssetType;

    virtual int32_t GetAssetId() const; // Just for boost to recognize it as polymorphic
};

namespace boost
{
    namespace serialization
    {
        template <class Archive>
        void serialize(Archive& ar, AssetDescriptor& descriptor, uint32_t version)
        {
            ar & descriptor.AssetId;
            ar & descriptor.AssetName;
            ar & descriptor.AssetType;
        }
    }
}

class Asset
{
public:
    void SetDescriptor(const std::shared_ptr<AssetDescriptor>& descriptor);
    template <class T>
    std::shared_ptr<T> GetDescriptor() const;

    std::shared_ptr<AssetDescriptor> GetDescriptor() const;

    std::string GetAssetName() const;

protected:
    std::shared_ptr<AssetDescriptor> m_Descriptor;
};

template <class T>
std::shared_ptr<T> Asset::GetDescriptor() const
{
    return std::static_pointer_cast<T>(m_Descriptor);
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(AssetDescriptor)
