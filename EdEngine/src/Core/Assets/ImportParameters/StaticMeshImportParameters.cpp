#include "StaticMeshImportParameters.h"

void StaticMeshImportParameters::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & JoinIdenticalVertices;
	archive & GenUVCoords;
	archive & CalculateTangentSpace;
	archive & FixInfacingNormals;
	archive & ImportAsOneMesh;
}
