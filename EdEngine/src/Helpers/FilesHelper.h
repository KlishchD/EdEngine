#pragma once

#include <string>
#include <filesystem>
#include "Core/Assets/Asset.h"

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "INVALID"
#endif

class FilesHelper
{
public:
    inline static const std::string ContentFolderPath = RESOURCES_PATH;
    inline static const std::string ContentFolderName = "resources";

    static std::string GetSaveExtensions(AssetType type);
    static std::string GetSavePath(const std::string& pathStr, AssetType type, const std::string& name = "");

    static uint64_t MakeRandomID();
};
