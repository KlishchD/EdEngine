#pragma once

#include "Core/Ed.h"
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

	UUID AssetId;
	std::string AssetName;
	AssetType AssetType;

	virtual void SetShouldHaveData(bool status);
	virtual bool ShouldHaveData() const;

	virtual bool HasData() const;
	virtual void ClaimData();
	virtual void UnclaimData();
private:
	int32_t m_DataClaims = 0;
	bool m_bShouldHaveData = false;
};

namespace boost
{
	namespace serialization
	{
		template <class Archive>
		void serialize(Archive& ar, AssetDescriptor& descriptor, uint32_t version)
		{
			ar & descriptor.AssetId;
			ar & descriptor.AssetName;
			ar & descriptor.AssetType;
		}
	}
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(AssetDescriptor)