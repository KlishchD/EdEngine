#pragma once

#include "AssetDescriptor.h"
#include "Core/Assets/ImportParameters/StaticMeshImportParameters.h"
#include "Utils/AssetUtils.h"

struct StaticSubmeshData
{
	StaticSubmeshData(StaticSubmeshData&& data) noexcept;
	StaticSubmeshData();

	std::string Name;

	std::vector<Vertex> Vertices;
	std::vector<int32_t> Indices;

	UUID MaterialID;
};

struct StaticMeshDescriptor : public AssetDescriptor
{
	StaticMeshImportParameters ImportParameters;
	std::vector<StaticSubmeshData> MeshData;

	virtual bool HasData() const override;
};


namespace boost
{
	namespace serialization
	{
		template <class Archive>
		void serialize(Archive& ar, StaticSubmeshData& data, uint32_t version)
		{
			ar & data.Name;
			ar & data.Vertices;
			ar & data.Indices;
			ar & data.MaterialID;
		}

		template <class Archive>
		void serialize(Archive& ar, StaticMeshDescriptor& descriptor, uint32_t version)
		{
			ar & boost::serialization::base_object<AssetDescriptor>(descriptor);

			ar & descriptor.ImportParameters;

			if (descriptor.ShouldHaveData() || (Archive::is_saving::value && descriptor.HasData()))
			{
				ar & descriptor.MeshData;
			}
		}
	}
}

BOOST_CLASS_EXPORT_KEY(StaticMeshDescriptor)
BOOST_CLASS_VERSION(StaticMeshDescriptor, 1)