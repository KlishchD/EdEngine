#pragma once

#include <stdint.h>
#include <string>

enum class AssetType : uint8_t
{
	Texture2D,
	CubeTexture,
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
