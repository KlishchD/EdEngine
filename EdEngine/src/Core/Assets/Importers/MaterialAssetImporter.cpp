#include "MaterialAssetImporter.h"
#include "Core/Assets/AssetManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include "Helpers/FilesHelper.h"
#include "Helpers/RenderingHelper.h"

MaterialAssetImporter::MaterialAssetImporter(std::shared_ptr<AssetManager> manager) : AssetImporter(manager)
{

}

std::shared_ptr<Asset> MaterialAssetImporter::Import(std::shared_ptr<AssetImportParameters> parameters)
{
	if (parameters->Path.empty()) return nullptr;

	if (const aiScene* scene = m_Importer.ReadFile(parameters->Path, 0))
	{
		if (scene->mNumMaterials)
		{
			m_Importer.FreeScene();

			return ImportMaterial(scene->mMaterials[0], parameters);
		}

		m_Importer.FreeScene();
	}

	return nullptr;
}

std::vector<std::shared_ptr<Asset>> MaterialAssetImporter::ImportMultiple(std::shared_ptr<AssetImportParameters> parameters)
{
	if (parameters->Path.empty()) return {};

	if (const aiScene* scene = m_Importer.ReadFile(parameters->Path, 0))
	{
		std::vector<std::shared_ptr<Asset>> materials;
		for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
		{
			materials.push_back(ImportMaterial(scene->mMaterials[i], parameters));
		}

		m_Importer.FreeScene();

		return materials;
	}

	return { };
}

std::shared_ptr<Material> MaterialAssetImporter::ImportMaterial(const aiMaterial* inMaterial, std::shared_ptr<AssetImportParameters> inParameters)
{
	const std::shared_ptr<MaterialImportParameters>& parameters = std::static_pointer_cast<MaterialImportParameters>(inParameters);

	std::shared_ptr<Material> material = ParseMaterial(inMaterial, inParameters->Path);

	material->SetImportParameters(parameters);

	std::string savePath = FilesHelper::GetSavePath(parameters->Path, AssetType::Material, material->GetName());

	Archive archive(savePath, ArchiveMode::Write);
	archive & material;

	m_Manager->RegisterAsset(material, savePath);

	return material;
}

std::shared_ptr<Material> MaterialAssetImporter::ParseMaterial(const aiMaterial* inMaterial, const std::string& materialPath)
{
	std::shared_ptr<Material> material = std::make_shared<Material>(inMaterial->GetName().C_Str());

	std::string root = std::filesystem::path(materialPath).remove_filename().string();

	aiString path;
	if (inMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == aiReturn_SUCCESS)
	{
		std::shared_ptr<Texture2DImportParameters> parameters = RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(root + path.data);
		std::shared_ptr<Texture2D> texture = m_Manager->GetImporter().Import<Texture2D>(AssetType::Texture2D, parameters);
		material->SetBaseColorTexture(texture);
	}

	if (inMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
	{
		std::shared_ptr<Texture2DImportParameters> parameters = RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(root + path.data);
		std::shared_ptr<Texture2D> texture = m_Manager->GetImporter().Import<Texture2D>(AssetType::Texture2D, parameters);
		material->SetBaseColorTexture(texture);
	}

	if (inMaterial->GetTexture(aiTextureType_HEIGHT, 0, &path) == aiReturn_SUCCESS)
	{
		std::shared_ptr<Texture2DImportParameters> parameters = RenderingHelper::GetDefaultNormalTexture2DImportParameters(root + path.data);
		std::shared_ptr<Texture2D> texture = m_Manager->GetImporter().Import<Texture2D>(AssetType::Texture2D, parameters);
		material->SetNormalTexture(texture);
	}

	if (inMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == aiReturn_SUCCESS)
	{
		std::shared_ptr<Texture2DImportParameters> parameters = RenderingHelper::GetDefaultRoughnessTexture2DImportParameters(root + path.data);
		std::shared_ptr<Texture2D> texture = m_Manager->GetImporter().Import<Texture2D>(AssetType::Texture2D, parameters);
		material->SetRoughnessTexture(texture);
	}

	if (inMaterial->GetTexture(aiTextureType_METALNESS, 0, &path) == aiReturn_SUCCESS)
	{
		std::shared_ptr<Texture2DImportParameters> parameters = RenderingHelper::GetDefaultMetalicTexture2DImportParameters(root + path.data);
		std::shared_ptr<Texture2D> texture = m_Manager->GetImporter().Import<Texture2D>(AssetType::Texture2D, parameters);
		material->SetMetalicTexture(texture);
	}

	{
		aiColor3D color(0.f, 0.f, 0.f);
		if (inMaterial->Get(AI_MATKEY_BASE_COLOR, color) == aiReturn_SUCCESS)
		{
			material->SetBaseColor({ color.r, color.g, color.b });
		}
	}

	{
		float roughness;
		if (inMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == aiReturn_SUCCESS)
		{
			material->SetRoughness(roughness);
		}
	}

	{
		float metalic;
		if (inMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metalic) == aiReturn_SUCCESS)
		{
			material->SetMetalic(metalic);
		}
	}

	return material;
}