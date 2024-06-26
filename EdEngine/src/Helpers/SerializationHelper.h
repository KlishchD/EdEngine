#pragma once

#include "Core/Ed.h"
#include "Core/Rendering/Types.h"
#include "Serializable.h"

class Asset;

class SerializationHelper
{
public:
    template<typename T> requires(std::is_base_of_v<Asset, T>)
    static std::shared_ptr<T> SerializeAsset(Archive& archive, std::shared_ptr<T> asset)
    {
        return std::static_pointer_cast<T>(SerializeAssetInternal(archive, std::static_pointer_cast<Asset>(asset)));
    }

private:
	static std::shared_ptr<Asset> SerializeAssetInternal(Archive& archive, std::shared_ptr<Asset> asset);
};
