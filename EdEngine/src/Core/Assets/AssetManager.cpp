#include "AssetManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>

#include "Descriptors/TextureDescriptor.h"
#include "Core/Assets/Descriptors/MaterialDescriptor.h"

#include "StaticMesh.h"
#include "Material.h"
#include "Core/Rendering/Textures/Texture.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Shader.h"

#include "Core/Scene.h"
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "Utils/RenderingHelper.h"
#include "Utils/AssetUtils.h"
#include "Utils/Files.h"
#include "Utils/stb_image.h"

#include "Core/Macros.h"

void AssetManager::Initialize(Engine* engine)
{
	ED_LOG(AssetManager, info, "Started initalizing")

    m_DescriptorsByType[AssetType::Texture2D] = std::vector<std::shared_ptr<AssetDescriptor>>();
    m_DescriptorsByType[AssetType::CubeTexture] = std::vector<std::shared_ptr<AssetDescriptor>>();
    m_DescriptorsByType[AssetType::Material] = std::vector<std::shared_ptr<AssetDescriptor>>();
    m_DescriptorsByType[AssetType::StaticMesh] = std::vector<std::shared_ptr<AssetDescriptor>>();

    LoadDescriptors(Files::ContentFolderPath);

    ED_LOG(AssetManager, info, "Finished initalizing")
}

void AssetManager::Deinitialize()
{
    for (auto& [id, asset] : m_Assets)
    {
        SaveAsset(asset);
    }

    for (const auto& [path, scene]: m_ScenesPaths)
    {
        SaveScene(path, scene);
    }
}

void AssetManager::LoadDescriptors(const std::string& contentPath)
{
    std::filesystem::recursive_directory_iterator iterator(contentPath);
    for (const std::filesystem::directory_entry& entry: iterator)
    {
        if (entry.is_directory()) continue;
        if (std::string extension = entry.path().extension().string(); AssetUtils::IsAssetExtension(extension))
        {
            std::string path = entry.path().string();
            switch (AssetUtils::GetAssetTypeFromExtension(extension))
            {
            case AssetType::Material:    LoadDescriptor<MaterialDescriptor>(path, false); break;
            case AssetType::Texture2D:   LoadDescriptor<Texture2DDescriptor>(path, false); break;
            case AssetType::StaticMesh:  LoadDescriptor<StaticMeshDescriptor>(path, false); break;
            default:
                break;
            }
        }
    }
}

std::vector<std::shared_ptr<StaticMeshDescriptor>> AssetManager::ImportMesh(const StaticMeshImportParameters& parameters)
{
    const std::string& meshPath = parameters.MeshPath;
    if (const aiScene* scene = m_Importer.ReadFile(meshPath.c_str(), GetImportParametersIntegerRepresentation(parameters)))
    {
        std::vector<std::shared_ptr<MaterialDescriptor>> materials = ImportMaterialInternal(scene, meshPath);
    
        std::vector<std::shared_ptr<StaticMeshDescriptor>> descriptors;

        if (parameters.ImportAsOneMesh)
        {
            std::shared_ptr<StaticMeshDescriptor> descriptor = std::make_shared<StaticMeshDescriptor>();
            descriptor->AssetId = UUIDs::random_generator()();
            descriptor->AssetName = std::filesystem::path(meshPath).filename().string();
            descriptor->AssetType = AssetType::StaticMesh;
    
            ParseNodesAndCombineInOneMesh(scene->mRootNode, scene, Transform(), materials, descriptor->MeshData);

            descriptor->ImportParameters = parameters;

            std::string savePath = Files::GetSavePath(meshPath, AssetType::StaticMesh);
            Serialization::SaveDescriptor(savePath, descriptor);
			AddDescriptor(descriptor, savePath);

            descriptors.push_back(descriptor);
        }
        else
        {
            std::vector<StaticSubmeshData> datas;
            ParseMeshesSeparately(scene->mRootNode, scene, materials, datas);
            
            for (StaticSubmeshData& data: datas)
            {
                std::shared_ptr<StaticMeshDescriptor> descriptor = std::make_shared<StaticMeshDescriptor>();
                descriptor->AssetId = UUIDs::random_generator()();
                descriptor->AssetName = std::filesystem::path(meshPath).filename().string();
                descriptor->AssetType = AssetType::StaticMesh;
    
                descriptor->ImportParameters = parameters;
                descriptor->MeshData.push_back(std::move(data));

                std::string savePath = Files::GetSavePath(meshPath, AssetType::StaticMesh, data.Name);
                Serialization::SaveDescriptor(savePath, descriptor);
				AddDescriptor(descriptor, savePath);

                descriptors.push_back(descriptor);
            }
        }

        m_Importer.FreeScene();

        return descriptors;
    }


    return {};
}

std::shared_ptr<Texture2DDescriptor> AssetManager::ImportTexture(const Texture2DImportParameters& parameters)
{
    const std::string& texturePath = parameters.Path;
    if (texturePath.empty()) return nullptr;

    std::shared_ptr<Texture2DDescriptor> descriptor = std::make_shared<Texture2DDescriptor>();
    descriptor->AssetId = UUIDs::random_generator()();
    descriptor->AssetName = std::filesystem::path(texturePath).filename().string();
    descriptor->AssetType = AssetType::Texture2D;

    descriptor->ImportParameters = parameters;

    stbi_set_flip_vertically_on_load(true);
    
    int32_t channals;
    Texture2DData& data = descriptor->Data;

    data.Data = stbi_load(texturePath.c_str(), &data.Width, &data.Height, &channals, Types::GetChannelNumber(parameters.Format));
    data.PixelSize = Types::GetPixelSize(parameters.Format);

    std::string savePath = Files::GetSavePath(texturePath, AssetType::Texture2D);
    Serialization::SaveDescriptor(savePath, descriptor);
	AddDescriptor(descriptor, savePath);

    return descriptor;
}

std::vector<std::shared_ptr<MaterialDescriptor>> AssetManager::ImportMaterial(const std::string& materialPath)
{
    std::vector<std::shared_ptr<MaterialDescriptor>> descriptors;
    
    if (const aiScene* scene = m_Importer.ReadFile(materialPath, 0))
    {
        ImportMaterialInternal(scene, materialPath);
    }

    return descriptors;
}

std::vector<std::shared_ptr<MaterialDescriptor>> AssetManager::ImportMaterialInternal(const aiScene* scene, const std::string& materialPath)
{
    std::vector<std::shared_ptr<MaterialDescriptor>> descriptors;    
    for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
    {
        const aiMaterial* material = scene->mMaterials[i];
            
        std::shared_ptr<MaterialDescriptor> descriptor = std::make_shared<MaterialDescriptor>();
        descriptor->AssetId = UUIDs::random_generator()();
        descriptor->AssetName = material->GetName().C_Str();
        descriptor->AssetType = AssetType::Material;

        aiString path;
        if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->BaseColorTextureID = ImportTexture(RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(path.data))->AssetId;
        }
        
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->BaseColorTextureID = ImportTexture(RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(path.C_Str()))->AssetId;
        }

        if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->NormalTextureID = ImportTexture(RenderingHelper::GetDefaultNormalTexture2DImportParameters(path.C_Str()))->AssetId;
        }

        if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->RoughnessTextureID = ImportTexture(RenderingHelper::GetDefaultRoughnessTexture2DImportParameters(path.C_Str()))->AssetId;
        }

        if (material->GetTexture(aiTextureType_METALNESS, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->MetalicTextureID = ImportTexture(RenderingHelper::GetDefaultMetalicTexture2DImportParameters(path.C_Str()))->AssetId;
        }
        
        material->Get(AI_MATKEY_BASE_COLOR, descriptor->BaseColor);
        material->Get(AI_MATKEY_ROUGHNESS_FACTOR, descriptor->Roughness);
        material->Get(AI_MATKEY_METALLIC_FACTOR, descriptor->Metalic);

        std::string savePath = Files::GetSavePath(materialPath, AssetType::Material, material->GetName().C_Str());
        Serialization::SaveDescriptor(savePath, descriptor);
		AddDescriptor(descriptor, savePath);

        descriptors.push_back(descriptor);
    }

    return descriptors;
}

std::vector<std::shared_ptr<Material>> AssetManager::GetAllMaterials() const
{
    std::vector<std::shared_ptr<Material>> materials;
    for (auto& [id, asset] : m_Assets)
    {
        if (std::shared_ptr<Material> material = std::dynamic_pointer_cast<Material>(asset))
        {
            materials.push_back(material);
        }
    }
    return materials;
}

std::shared_ptr<MaterialDescriptor> AssetManager::CreateMaterial(const std::string& materialPath)
{
    if (materialPath.empty()) return nullptr;

    std::shared_ptr<MaterialDescriptor> descriptor = std::make_shared<MaterialDescriptor>();
    descriptor->AssetId = UUIDs::random_generator()();
    descriptor->AssetName = std::filesystem::path(materialPath).filename().string();
    descriptor->AssetType = AssetType::Material;

    descriptor->BaseColor = glm::vec4(1.0f);
    descriptor->Roughness = 0.5f;
    descriptor->Metalic = 0.5f;

    std::string savePath = Files::GetSavePath(materialPath, AssetType::Material);
    
    Serialization::SaveDescriptor(savePath, descriptor);
	AddDescriptor(descriptor, savePath);

    return descriptor;
}

std::shared_ptr<StaticMesh> AssetManager::LoadMesh(const std::shared_ptr<StaticMeshDescriptor>& descriptor)
{
    if (!descriptor) return nullptr;
    
    ED_LOG(AssetManager, info, "Started loading mesh: {}", descriptor->AssetName)
    
    if (m_Assets.count(descriptor->AssetId)) // TODO : may be change to find ? 
    {
        ED_LOG(AssetManager, info, "Finished loading mesh: {}", descriptor->AssetName)
        return std::static_pointer_cast<StaticMesh>(m_Assets[descriptor->AssetId]);
    }
    
    Serialization::LoadDescriptor(m_AssetIdToDescriptorPath[descriptor->AssetId], descriptor);

    std::shared_ptr<StaticMesh> mesh = AssetUtils::CreateStaticMesh(descriptor);
    
    m_Assets[descriptor->AssetId] = mesh;

	ED_LOG(AssetManager, info, "Finished loading mesh: {}", descriptor->AssetName)
    return mesh;
}

std::shared_ptr<Texture2D> AssetManager::LoadTexture(const std::shared_ptr<Texture2DDescriptor>& descriptor)
{
    if (!descriptor) return nullptr;
    
    ED_LOG(AssetManager, info, "Started loading texture: {}", descriptor->AssetName)
    if (m_Assets.count(descriptor->AssetId)) // TODO : may be change to find ? 
    {
		ED_LOG(AssetManager, info, "Finished loading texture: {}", descriptor->AssetName)
        return std::static_pointer_cast<Texture2D>(m_Assets[descriptor->AssetId]);
    }

    Serialization::LoadDescriptor(m_AssetIdToDescriptorPath[descriptor->AssetId], descriptor);

    std::shared_ptr<Texture2D> texture = RenderingHelper::CreateTexture2D(descriptor);
    m_Assets[descriptor->AssetId] = texture;
    
	ED_LOG(AssetManager, info, "Finished loading texture: {}", descriptor->AssetName)
    return texture;
}

std::shared_ptr<Texture2D> AssetManager::LoadTexture(const Texture2DImportParameters& parameters)
{
    std::shared_ptr<Texture2DDescriptor> descriptor = ImportTexture(parameters);
    return LoadTexture(descriptor);
}

std::shared_ptr<Material> AssetManager::LoadMaterial(const std::shared_ptr<MaterialDescriptor>& descriptor)
{
    if (!descriptor) return nullptr;

	ED_LOG(AssetManager, info, "Started loading material: {}", descriptor->AssetName)

    if (m_Assets.count(descriptor->AssetId))
    {
		ED_LOG(AssetManager, info, "Finished loading material: {}", descriptor->AssetName)
        return std::static_pointer_cast<Material>(m_Assets[descriptor->AssetId]);
    }

    Serialization::LoadDescriptor(m_AssetIdToDescriptorPath[descriptor->AssetId], descriptor);

    std::shared_ptr<Material> material = AssetUtils::CreateMaterial(descriptor);
    m_Assets[descriptor->AssetId] = material;

	ED_LOG(AssetManager, info, "Finished loading material: {}", descriptor->AssetName)
    return material;
}

std::shared_ptr<StaticMesh> AssetManager::LoadMesh(UUID assetId)
{
    if (assetId.is_nil()) return nullptr;
    return LoadMesh(std::static_pointer_cast<StaticMeshDescriptor>(m_Descriptors[assetId]));
}

std::shared_ptr<Texture2D> AssetManager::LoadTexture(UUID assetId)
{
    if (assetId.is_nil()) return nullptr;
    return LoadTexture(std::static_pointer_cast<Texture2DDescriptor>(m_Descriptors[assetId]));
}

std::shared_ptr<Material> AssetManager::LoadMaterial(UUID assetId)
{
    if (assetId.is_nil()) return nullptr;
    return LoadMaterial(std::static_pointer_cast<MaterialDescriptor>(m_Descriptors[assetId]));
}

std::shared_ptr<AssetDescriptor> AssetManager::GetAssetDescriptor(const std::string& path) const
{
    if (m_DescriptorPathToAssetId.count(path))
    {
        return m_Descriptors.at(m_DescriptorPathToAssetId.at(path));
    }
    return nullptr;
}

std::shared_ptr<AssetDescriptor> AssetManager::GetAssetDescriptor(UUID assetId)
{
    return assetId.is_nil() ? nullptr : m_Descriptors[assetId];
}

std::shared_ptr<Scene> AssetManager::CreateScene(const std::string& path)
{
    ED_LOG(AssetManager, info, "Started creating scene: {}", path)
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	m_ScenesPaths.emplace_back(path, scene);
	ED_LOG(AssetManager, info, "Finished creating scene: {}", path)
	return scene;
}

std::shared_ptr<Scene> AssetManager::LoadScene(const std::string& path)
{
    ED_LOG(AssetManager, info, "Started loading scene: {}", path)
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();

    std::filesystem::directory_entry entry(path);

    if (!entry.exists() || entry.is_directory())
    {
        ED_LOG(AssetManager, info, "Couldn't find the scene: {}", path)
        return nullptr;
    }

    {
        std::ifstream file(path, std::ios_base::binary);
        boost::archive::text_iarchive ia(file);
        ia >> *scene;
    }
    
    m_ScenesPaths.emplace_back(path, scene);
    
    ED_LOG(AssetManager, info, "Finished loading scene: {}", path)
    return scene;
}

void AssetManager::SaveAsset(const std::shared_ptr<Asset>& asset)
{
	std::shared_ptr<AssetDescriptor> descriptor = asset->GetDescriptor();

    std::string path = m_AssetIdToDescriptorPath[descriptor->AssetId];

	ED_LOG(AssetManager, info, "Started saving descriptor: {}", path)

	asset->SyncDescriptor();

	std::ofstream file(path, std::ios_base::binary);
	boost::archive::binary_oarchive oa(file);

	switch (descriptor->AssetType)
	{
	case AssetType::Texture2D:   
        oa & *std::static_pointer_cast<Texture2DDescriptor>(descriptor);
        break;
	case AssetType::CubeTexture:
        oa & *std::static_pointer_cast<CubeTextureDescriptor>(descriptor);
        break;
	case AssetType::Material:
        oa & *std::static_pointer_cast<MaterialDescriptor>(descriptor);
        break;
	case AssetType::StaticMesh:
        oa & *std::static_pointer_cast<StaticMeshDescriptor>(descriptor);
        break;
	default:
		ED_ASSERT(0, "This asset type cannot be serialized");
		break;
	}

	ED_LOG(AssetManager, info, "Finished saving descriptor: {}", path)
}

void AssetManager::SaveScene(const std::string& path, const std::shared_ptr<Scene>& scene)
{
	ED_LOG(AssetManager, info, "Started saving scene: {}", path)

    std::ofstream file(path, std::ios_base::binary);
    boost::archive::text_oarchive oa(file);
    oa & *scene;

	ED_LOG(AssetManager, info, "Finished saving scene: {}", path)
}
    
template<class T>
std::shared_ptr<AssetDescriptor> AssetManager::LoadDescriptor(const std::string& path, bool bLoadData)
{
    ED_LOG(AssetManager, info, "Started loading descriptor {}", path)

    std::shared_ptr<T> descriptor = std::make_shared<T>();
    Serialization::LoadDescriptor(path, descriptor, bLoadData);
    AddDescriptor(descriptor, path);

	ED_LOG(AssetManager, info, "Finished loading descriptor {}", path)

    return descriptor;
}

void AssetManager::AddDescriptor(const std::shared_ptr<AssetDescriptor>& descriptor, const std::string& path)
{
    if (!m_Assets.count(descriptor->AssetId)) {
        m_AssetIdToDescriptorPath[descriptor->AssetId] = path;
        m_DescriptorPathToAssetId[path] = descriptor->AssetId;
        m_Descriptors[descriptor->AssetId] = descriptor;
        m_DescriptorsByType[descriptor->AssetType].push_back(descriptor);
    }
}

Transform AssetManager::ParseSubmeshTransformation(aiNode* node)
{
    aiVector3D scale, rotation, translation;
    node->mTransformation.Decompose(scale, rotation, translation);
                
    return Transform(*(glm::vec3*)&translation, glm::quat(*(glm::vec3*)&rotation), *(glm::vec3*)&scale);
}

int32_t AssetManager::GetImportParametersIntegerRepresentation(const StaticMeshImportParameters& parameters)
{
    int32_t result = aiProcess_Triangulate;
    if (parameters.JoinIdenticalVertices) result |= aiProcess_JoinIdenticalVertices;
    if (parameters.GenUVCoords) result |= aiProcess_GenUVCoords;
    if (parameters.CalculateTangentSpace) result |= aiProcess_CalcTangentSpace;
    if (parameters.FixInfacingNormals) result |= aiProcess_FixInfacingNormals;
    return result;
}

void AssetManager::ParseNodesAndCombineInOneMesh(aiNode* node, const aiScene* scene, const Transform& parentTransform, const std::vector<std::shared_ptr<MaterialDescriptor>>& materials, std::vector<StaticSubmeshData>& datas)
{
    Transform nodeTransformation = parentTransform + ParseSubmeshTransformation(node);

    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        datas.push_back(ParseMesh(mesh, nodeTransformation, materials, false));
    }
    
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        ParseNodesAndCombineInOneMesh(node->mChildren[i], scene, nodeTransformation, materials, datas);
    }
}

void AssetManager::ParseMeshesSeparately(aiNode* node, const aiScene* scene, const std::vector<std::shared_ptr<MaterialDescriptor>>& materials, std::vector<StaticSubmeshData>& datas)
{
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        datas.push_back(ParseMesh(mesh, Transform(), materials, false));
    }
    
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        ParseMeshesSeparately(node->mChildren[i], scene, materials, datas);
    }
}

StaticSubmeshData AssetManager::ParseMesh(aiMesh* mesh, const Transform& transform, const std::vector<std::shared_ptr<MaterialDescriptor>>& materials, bool dropTranslation)
{
    StaticSubmeshData data;
    
    glm::mat4 world = transform.GetMatrix();
    glm::mat4 normal = transform.GetInversedTransposedMatrix();
    
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;

        aiVector3D& position = mesh->mVertices[i];
        vertex.Position = world * glm::vec4(position.x, position.y, position.z, !dropTranslation);
        
        if (mesh->mColors[0]) {
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

        data.Vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
    {
        for (int32_t k = 0; k < 3; ++k)
        {
            data.Indices.push_back(mesh->mFaces[i].mIndices[k]);
        }
    }

    data.Name = mesh->mName.C_Str();
    
    if (mesh->mMaterialIndex < materials.size())
    {
        data.MaterialID = materials[mesh->mMaterialIndex]->AssetId;
    }
    
    return data;
}
