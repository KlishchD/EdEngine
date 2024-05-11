#include "StaticMeshImportParameters.h"

void StaticMeshImportParameters::Serialize(Archive& archive)
{
	AssetImportParameters::Serialize(archive);

	archive & JoinIdenticalVertices;
	archive & GenUVCoords;
	archive & CalculateTangentSpace;
	archive & FixInfacingNormals;
	archive & ImportAsOneMesh;
}
