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
    inline static const std::string PlayRecordingsFolderPath = ContentFolderPath + "\\PlayRecordings";
    inline static const std::string PlayRecordingsExtension = ".edprec";

    static void CreateContentDirectory(std::string realtivePath);
    static void CreateDirectory(std::string realtivePath);

    // TODO: Move all files info here

    static std::string GetSaveExtensions(AssetType type);
    static std::string GetSavePath(const std::string& pathStr, AssetType type, const std::string& name = "");

    static uint64_t MakeRandomID();

    static bool IsPlayRecordingExtension(const std::string& extension) { return extension == PlayRecordingsExtension; }
    static std::string MakeRecordingsPath(const std::string& name) { return PlayRecordingsFolderPath + "\\" + name + PlayRecordingsExtension; }
};
