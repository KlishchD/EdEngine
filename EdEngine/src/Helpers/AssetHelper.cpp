#include "AssetHelper.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/StaticMesh.h"
#include "Core/Rendering/Types.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Buffers/VertexBufferLayout.h"
#include "RenderingHelper.h"
#include "Core/Macros.h"

AssetType AssetHelper::GetAssetTypeFromExtension(const std::string& extension)
{
	if (extension == ".edmesh")
	{
		return AssetType::StaticMesh;
	}
	else if (extension == ".edmaterial")
	{
		return AssetType::Material;
	}
	else if (extension == ".edtexture")
	{
		return AssetType::Texture2D;
	}

    ED_ASSERT(0, "Unknown extension")
}

bool AssetHelper::IsAssetExtension(const std::string& extension)
{
	return extension == ".edmesh" || extension == ".edmaterial" || extension == ".edtexture";
}

std::string AssetHelper::GetAssetNameLable(std::shared_ptr<Asset> asset)
{
	return asset ? asset->GetName() + "##" + std::to_string((int32_t)asset.get()) : "None";
}
