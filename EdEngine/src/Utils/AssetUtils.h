#pragma once

#include "Core/Ed.h"

struct StaticMeshDescriptor;
struct StaticSubmeshData;
struct MaterialDescriptor;
struct AssetDescriptor;

class StaticSubmesh;
class StaticMesh;
class Material;
class Texture2D;

enum class AssetType : uint8_t;

class AssetUtils
{
public:
    static std::shared_ptr<Material> CreateMaterial(const std::shared_ptr<MaterialDescriptor>& descriptor);

    static std::shared_ptr<StaticMesh> CreateStaticMesh(const std::shared_ptr<StaticMeshDescriptor>& descriptor);
    static std::shared_ptr<StaticMesh> CreateStaticMesh(UUID staticMeshId);

    static std::shared_ptr<StaticSubmesh> CreateStaticSubmesh(const StaticSubmeshData& data);

    static AssetType GetAssetTypeFromExtension(const std::string& extension);
    static bool IsAssetExtension(const std::string& extension);

	static std::string GetSelectTextureLable(const std::shared_ptr<Texture2D>& texture);
	static std::string GetAssetNameLable(const std::shared_ptr<AssetDescriptor>& descriptor);
};
