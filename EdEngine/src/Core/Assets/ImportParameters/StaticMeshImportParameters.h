#pragma once

#include "AssetImportParameters.h"
#include <string>

struct StaticMeshImportParameters : public AssetImportParameters
{
	bool JoinIdenticalVertices = true;
	bool GenUVCoords = true;
	bool CalculateTangentSpace = true;
	bool FixInfacingNormals = true;

	bool ImportAsOneMesh = true;
	bool ImportMaterials = true;

	virtual void Serialize(Archive& archive) override;
};
