#pragma once

#include "Core/Ed.h"

enum class AssetType : uint8_t;

class AssetUtils
{
public:
    static AssetType GetAssetTypeFromExtension(const std::string& extension);
    static bool IsAssetExtension(const std::string& extension);

	static std::string GetAssetNameLable(std::shared_ptr<Asset> asset);
};
