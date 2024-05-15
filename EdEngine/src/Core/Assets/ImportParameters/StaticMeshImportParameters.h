#pragma once

#include "AssetImportParameters.h"
#include <string>

ED_CLASS(StaticMeshImportParameters) : public AssetImportParameters
{
	ED_CLASS_BODY(StaticMeshImportParameters, AssetImportParameters)
public:
	bool JoinIdenticalVertices = true;
	bool GenUVCoords = true;
	bool CalculateTangentSpace = true;
	bool FixInfacingNormals = true;

	bool ImportAsOneMesh = true;
	bool ImportMaterials = true;

	virtual void Serialize(Archive& archive) override;
};
