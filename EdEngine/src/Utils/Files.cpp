#include "Files.h"

#include <filesystem>

std::string Files::GetSaveExtensions(AssetType type)
{
    switch (type)
    {
    case AssetType::Texture2D:
    case AssetType::CubeTexture: return ".edtexture";
    case AssetType::Material: return ".edmaterial";
    case AssetType::StaticMesh: return ".edmesh";
    }
    return "";
}

std::string Files::GetSavePath(const std::string& pathStr, AssetType type, const std::string& name)
{
    std::filesystem::path path(pathStr);

    std::string filename = std::filesystem::path(name.empty() ? path : name).stem().replace_extension().string();
    std::string directoryPath = path.remove_filename().string();

    return directoryPath + filename + GetSaveExtensions(type);
}