#pragma once

#include <assimp/Importer.hpp>
#include "AssetImporter.h"
#include "Core/Assets/StaticMesh.h"
#include "Core/Assets/ImportParameters/StaticMeshImportParameters.h"
#include "Core/Math/Transform.h"

struct aiNode;
struct aiScene;
struct aiMesh;

class StaticMeshImporter : public AssetImporter
{
public:
	StaticMeshImporter(std::shared_ptr<AssetManager> manager);

	virtual std::shared_ptr<Asset> Import(std::shared_ptr<AssetImportParameters> parameters);
	virtual std::vector<std::shared_ptr<Asset>> ImportMultiple(std::shared_ptr<AssetImportParameters> inParameters);

protected:
	int32_t GetParametersIntegerRepresentation(std::shared_ptr<StaticMeshImportParameters> parameters);

	void ParseNodesAndCombineInOneMesh(aiNode* node, const aiScene* scene, const Transform& parentTransform, const std::vector<std::shared_ptr<Material>>& materials, std::vector<std::shared_ptr<StaticSubmesh>>& submeshes);
	void ParseMeshesSeparately(aiNode* node, const aiScene* scene, const std::vector<std::shared_ptr<Material>>& materials, std::vector<std::shared_ptr<StaticSubmesh>>& submeshes);

	Transform ParseSubmeshTransformation(aiNode* node);
	std::shared_ptr<StaticSubmesh> ParseSubmesh(aiMesh* mesh, const Transform& transform, const std::vector<std::shared_ptr<Material>>& materials, bool dropTranslation);

	std::shared_ptr<StaticMesh> CreateMesh(std::shared_ptr<StaticSubmesh> submesh, std::shared_ptr<StaticMeshImportParameters> parameters, const std::string& name);
	std::shared_ptr<StaticMesh> CreateMesh(std::vector<std::shared_ptr<StaticSubmesh>>&& submeshes, std::shared_ptr<StaticMeshImportParameters> parameters, const std::string& name);
protected:
	Assimp::Importer m_Importer;
};
