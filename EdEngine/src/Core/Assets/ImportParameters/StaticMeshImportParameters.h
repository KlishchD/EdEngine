#pragma once

#include <string>

struct StaticMeshImportParameters
{
	std::string MeshPath;

	bool JoinIdenticalVertices = true;
	bool GenUVCoords = true;
	bool CalculateTangentSpace = true;
	bool FixInfacingNormals = true;

	bool ImportAsOneMesh = true;
	bool ImportMaterials = true;
};


namespace boost
{
	namespace serialization
	{
		template <class Archive>
		void serialize(Archive& ar, StaticMeshImportParameters& parameters, uint32_t version)
		{
			ar & parameters.MeshPath;

			ar & parameters.JoinIdenticalVertices;
			ar & parameters.GenUVCoords;
			ar & parameters.CalculateTangentSpace;
			ar & parameters.FixInfacingNormals;
			ar & parameters.ImportAsOneMesh;
		}
	}
}