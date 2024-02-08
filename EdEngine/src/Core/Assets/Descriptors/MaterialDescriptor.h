#pragma once

#include "AssetDescriptor.h"
#include <glm/ext/vector_float3.hpp>

struct MaterialDescriptor : public AssetDescriptor
{
	std::string MaterialPath;
	std::string ShaderPath;

	glm::vec3 BaseColor = glm::vec3(1.0f);
	float Roughness = 1.0f;
	float Metalic = 1.0f;
	float Emission = 0.0f;

	UUID BaseColorTextureID;
	UUID NormalTextureID;
	UUID RoughnessTextureID;
	UUID MetalicTextureID;
};

namespace boost
{
	namespace serialization
	{
		template <class Archive>
		void serialize(Archive& ar, MaterialDescriptor& descriptor, uint32_t version)
		{
			ar & boost::serialization::base_object<AssetDescriptor>(descriptor);

			ar & descriptor.BaseColor.x;
			ar & descriptor.BaseColor.y;
			ar & descriptor.BaseColor.z;

			ar & descriptor.Roughness;
			ar & descriptor.Metalic;
			ar & descriptor.Emission;

			ar & descriptor.ShaderPath;
			ar & descriptor.BaseColorTextureID;
			ar & descriptor.NormalTextureID;
			ar & descriptor.RoughnessTextureID;
			ar & descriptor.MetalicTextureID;
		}
	}
}

BOOST_CLASS_EXPORT_KEY(MaterialDescriptor)
BOOST_CLASS_VERSION(MaterialDescriptor, 1)