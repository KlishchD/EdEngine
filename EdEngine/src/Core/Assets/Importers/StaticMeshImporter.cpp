#include "StaticMeshImporter.h"
#include "Core/Assets/AssetManager.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include "Helpers/FilesHelper.h"

StaticMeshImporter::StaticMeshImporter(std::shared_ptr<AssetManager> manager) : AssetImporter(manager)
{

}

std::shared_ptr<Asset> StaticMeshImporter::Import(std::shared_ptr<AssetImportParameters> parameters)
{
	std::vector<std::shared_ptr<Asset>> assets = ImportMultiple(parameters);
	return assets.empty() ? nullptr : assets[0];
}

std::vector<std::shared_ptr<Asset>> StaticMeshImporter::ImportMultiple(std::shared_ptr<AssetImportParameters> inParameters)
{
	std::shared_ptr<StaticMeshImportParameters> parameters = std::static_pointer_cast<StaticMeshImportParameters>(inParameters);
	const std::string& path = parameters->Path;

	std::string name = std::filesystem::path(path).filename().string();
	std::vector<std::shared_ptr<Asset>> meshes;

	if (const aiScene* scene = m_Importer.ReadFile(path.c_str(), GetParametersIntegerRepresentation(parameters)))
	{
		std::shared_ptr<MaterialImportParameters> materialParameter = std::make_shared<MaterialImportParameters>();
		materialParameter->Path = parameters->Path;
		std::vector<std::shared_ptr<Material>> materials = m_Manager->GetImporter().ImportMultiple<Material>(AssetType::Material, materialParameter);

		std::vector<std::shared_ptr<StaticSubmesh>> submeshes;

		if (parameters->ImportAsOneMesh)
		{
			ParseNodesAndCombineInOneMesh(scene->mRootNode, scene, Transform(), materials, submeshes);
			std::shared_ptr<StaticMesh> mesh = CreateMesh(std::move(submeshes), parameters, name);
			meshes.push_back(mesh);
		}
		else
		{
			ParseMeshesSeparately(scene->mRootNode, scene, materials, submeshes);
			
			for (std::shared_ptr<StaticSubmesh> submesh : submeshes)
			{
				std::shared_ptr<StaticMesh> mesh = CreateMesh(submesh, parameters, name);
				meshes.push_back(mesh);

			}
		}

		m_Importer.FreeScene();
	}

	return meshes;
}


int32_t StaticMeshImporter::GetParametersIntegerRepresentation(std::shared_ptr<StaticMeshImportParameters> parameters)
{
	int32_t result = aiProcess_Triangulate;
	if (parameters->JoinIdenticalVertices) result |= aiProcess_JoinIdenticalVertices;
	if (parameters->GenUVCoords) result |= aiProcess_GenUVCoords;
	if (parameters->CalculateTangentSpace) result |= aiProcess_CalcTangentSpace;
	if (parameters->FixInfacingNormals) result |= aiProcess_FixInfacingNormals;
	return result;
}

void StaticMeshImporter::ParseNodesAndCombineInOneMesh(aiNode* node, const aiScene* scene, const Transform& parentTransform, const std::vector<std::shared_ptr<Material>>& materials, std::vector<std::shared_ptr<StaticSubmesh>>& submeshes)
{
	Transform nodeTransformation = parentTransform + ParseSubmeshTransformation(node);

	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::shared_ptr<StaticSubmesh> submesh = ParseSubmesh(mesh, nodeTransformation, materials, false);
		submeshes.push_back(submesh);
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		ParseNodesAndCombineInOneMesh(node->mChildren[i], scene, nodeTransformation, materials, submeshes);
	}
}

void StaticMeshImporter::ParseMeshesSeparately(aiNode* node, const aiScene* scene, const std::vector<std::shared_ptr<Material>>& materials, std::vector<std::shared_ptr<StaticSubmesh>>& submeshes)
{
	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::shared_ptr<StaticSubmesh> submesh = ParseSubmesh(mesh, Transform(), materials, false);
		submeshes.push_back(submesh);
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		ParseMeshesSeparately(node->mChildren[i], scene, materials, submeshes);
	}
}

Transform StaticMeshImporter::ParseSubmeshTransformation(aiNode* node)
{
	aiVector3D scale, rotation, translation;
	node->mTransformation.Decompose(scale, rotation, translation);

	return Transform(*(glm::vec3*)&translation, glm::quat(*(glm::vec3*)&rotation), *(glm::vec3*)&scale);
}

std::shared_ptr<StaticMesh> StaticMeshImporter::CreateMesh(std::shared_ptr<StaticSubmesh> submesh, std::shared_ptr<StaticMeshImportParameters> parameters, const std::string& name)
{
	std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(name);
	mesh->AddSubmesh(submesh);
	
	mesh->SetImportParameters(parameters);
	
	std::string savePath = FilesHelper::GetSavePath(parameters->Path, AssetType::StaticMesh, submesh->GetName());

	Archive archive(savePath, ArchiveMode::Write);
	archive & mesh;
	
	m_Manager->RegisterAsset(mesh, savePath);
	
	return mesh;
}

std::shared_ptr<StaticMesh> StaticMeshImporter::CreateMesh(std::vector<std::shared_ptr<StaticSubmesh>>&& submeshes, std::shared_ptr<StaticMeshImportParameters> parameters, const std::string& name)
{
	std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(name);
	mesh->SetSubmeshes(std::move(submeshes));
	
	mesh->SetImportParameters(parameters);
	
	std::string savePath = FilesHelper::GetSavePath(parameters->Path, AssetType::StaticMesh);

	Archive archive(savePath, ArchiveMode::Write);
	archive & mesh;
	
	m_Manager->RegisterAsset(mesh, savePath);
	
	return mesh;
}

std::shared_ptr<StaticSubmesh> StaticMeshImporter::ParseSubmesh(aiMesh* mesh, const Transform& transform, const std::vector<std::shared_ptr<Material>>& materials, bool dropTranslation)
{
	std::shared_ptr<StaticSubmesh> submesh = std::make_shared<StaticSubmesh>(mesh->mName.C_Str());

	glm::mat4 world = transform.GetMatrix();
	glm::mat4 normal = transform.GetInversedTransposedMatrix();

	std::vector<Vertex> vertices(mesh->mNumVertices);
	for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex& vertex = vertices[i];

		aiVector3D& position = mesh->mVertices[i];
		vertex.Position = world * glm::vec4(position.x, position.y, position.z, !dropTranslation);

		if (mesh->mColors[0])
		{
			vertex.Color = *(glm::vec4*)&mesh->mColors[0][i];
		}

		if (mesh->mTextureCoords[0])
		{
			vertex.TextureCoordinates = *(glm::vec3*)&mesh->mTextureCoords[0][i];
		}

		if (mesh->mNormals)
		{
			vertex.Normal = normal * glm::vec4(*(glm::vec3*)&mesh->mNormals[i], 0.0f);
		}

		if (mesh->mTangents)
		{
			vertex.Tangent = normal * glm::vec4(*(glm::vec3*)&mesh->mTangents[i], 0.0f);
		}

		if (mesh->mBitangents)
		{
			vertex.Bitangent = normal * glm::vec4(*(glm::vec3*)&mesh->mBitangents[i], 0.0f);
		}
	}

	std::vector<int32_t> indices(mesh->mNumFaces * 3);
	for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
	{
		for (int32_t k = 0; k < 3; ++k)
		{
			indices[i * 3 + k] = mesh->mFaces[i].mIndices[k];
		}
	}

	submesh->SetData(std::move(vertices), std::move(indices));

	if (mesh->mMaterialIndex < materials.size())
	{
		submesh->SetMaterial(materials[mesh->mMaterialIndex]);
	}

	return submesh;
}