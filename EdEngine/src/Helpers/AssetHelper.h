#pragma once

#include "Core/Ed.h"

class Asset;
enum class AssetType : uint8_t;

class AssetHelper
{
public:
    static AssetType GetAssetTypeFromExtension(const std::string& extension);
    static bool IsAssetExtension(const std::string& extension);

	static std::string GetAssetNameLable(std::shared_ptr<Asset> asset);
};
