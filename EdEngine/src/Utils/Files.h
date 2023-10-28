#pragma once

#include <string>
#include <filesystem>
#include "Core/Assets/Asset.h"

class Files
{
public:
    inline static const std::string ContentFolderPath = std::filesystem::current_path().parent_path().string() + "\\resources\\";
    inline static const std::string ContentFolderName = "resources";

    static std::string GetSaveExtensions(AssetType type);
    static std::string GetSavePath(const std::string& pathStr, AssetType type, const std::string& name = "");
};
