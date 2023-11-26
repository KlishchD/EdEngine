#include "AssetManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>

#include "Shader.h"
#include "StaticMesh.h"
#include "Texture2D.h"
#include "Core/Scene.h"
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "Utils/AssetUtils.h"
#include "Utils/Files.h"
#include "Utils/stb_image.h"

#include "Core/Macros.h"

void AssetManager::Initialize(Engine* engine)
{
	ED_LOG(AssetManager, info, "Started initalizing")

    m_DescriptorsByType[AssetType::Texture2D] = std::vector<std::shared_ptr<AssetDescriptor>>();
    m_DescriptorsByType[AssetType::Texture3D] = std::vector<std::shared_ptr<AssetDescriptor>>();
    m_DescriptorsByType[AssetType::Material] = std::vector<std::shared_ptr<AssetDescriptor>>();
    m_DescriptorsByType[AssetType::StaticMesh] = std::vector<std::shared_ptr<AssetDescriptor>>();

    LoadDescriptors(Files::ContentFolderPath);

    ED_LOG(AssetManager, info, "Finished initalizing")
}

void AssetManager::Deinitialize()
{
    // TODO: Add dirty descriptors save logic
                                 
    for (const std::shared_ptr<MaterialDescriptor>& descriptor: GetMaterialDescriptors())
    {
        SaveDescriptor(m_AssetIdToDescriptorPath[descriptor->AssetId], descriptor, false);
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
        if (IsAssetExtension(entry.path().extension().string()))
        {
            LoadDescriptor(entry.path(), false);
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
            descriptor->AssetId = m_FreeAssetId++;
            descriptor->AssetName = std::filesystem::path(meshPath).filename().string();
            descriptor->AssetType = AssetType::StaticMesh;
    
            ParseNodesAndCombineInOneMesh(scene->mRootNode, scene, Transform(), materials, descriptor->MeshData);

            descriptor->ImportParameters = parameters;

            std::string savePath = Files::GetSavePath(meshPath, AssetType::StaticMesh);
            SaveDescriptor(savePath, descriptor);

            descriptors.push_back(descriptor);
        }
        else
        {
            std::vector<StaticSubmeshData> datas;
            ParseMeshesSeparately(scene->mRootNode, scene, materials, datas);
            
            for (StaticSubmeshData& data: datas)
            {
                std::shared_ptr<StaticMeshDescriptor> descriptor = std::make_shared<StaticMeshDescriptor>();
                descriptor->AssetId = m_FreeAssetId++;
                descriptor->AssetName = std::filesystem::path(meshPath).filename().string();
                descriptor->AssetType = AssetType::StaticMesh;
    
                descriptor->ImportParameters = parameters;
                descriptor->MeshData.push_back(std::move(data));

                std::string savePath = Files::GetSavePath(meshPath, AssetType::StaticMesh, data.Name);
                SaveDescriptor(savePath, descriptor);

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
    const std::string& texturePath = parameters.ImagePath; 
    if (texturePath.empty()) return nullptr;

    std::shared_ptr<Texture2DDescriptor> descriptor = std::make_shared<Texture2DDescriptor>();
    descriptor->AssetId = m_FreeAssetId++;
    descriptor->AssetName = std::filesystem::path(texturePath).filename().string();
    descriptor->AssetType = AssetType::Texture2D;

    descriptor->ImportParameters = parameters;

    stbi_set_flip_vertically_on_load(true);
    
    Texture2DData& data = descriptor->TextureData;
    data.Data = stbi_load(texturePath.c_str(), &data.Width, &data.Height, &data.Channels, 4);

    std::string savePath = Files::GetSavePath(texturePath, AssetType::Texture2D);
    SaveDescriptor(savePath, descriptor);

    if (data.Data) {
        stbi_image_free(data.Data);
    }

    data.Data = nullptr;
    
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
        descriptor->AssetId = m_FreeAssetId++;
        descriptor->AssetName = material->GetName().C_Str();
        descriptor->AssetType = AssetType::Material;

        aiString path;
        if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->BaseColorTextureID = ImportTexture(Texture2DImportParameters::GetDefaultBaseColorTextureImportParameters(path.data))->AssetId;
        }
        
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->BaseColorTextureID = ImportTexture(Texture2DImportParameters::GetDefaultBaseColorTextureImportParameters(path.C_Str()))->AssetId;
        }

        if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->NormalTextureID = ImportTexture(Texture2DImportParameters::GetDefaultNormalTextureImportParameters(path.C_Str()))->AssetId;
        }

        if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->RoughnessTextureID = ImportTexture(Texture2DImportParameters::GetDefaultRoughnessTextureImportParameters(path.C_Str()))->AssetId;
        }

        if (material->GetTexture(aiTextureType_METALNESS, 0, &path) == aiReturn_SUCCESS)
        {
            descriptor->MetalicTextureID = ImportTexture(Texture2DImportParameters::GetDefaultMetalicTextureImportParameters(path.C_Str()))->AssetId;
        }
        
        material->Get(AI_MATKEY_BASE_COLOR, descriptor->BaseColor);
        material->Get(AI_MATKEY_ROUGHNESS_FACTOR, descriptor->Roughness);
        material->Get(AI_MATKEY_METALLIC_FACTOR, descriptor->Metalic);

        std::string savePath = Files::GetSavePath(materialPath, AssetType::Material, material->GetName().C_Str());
        SaveDescriptor(savePath, descriptor);

        descriptors.push_back(descriptor);
    }

    return descriptors;
}

std::shared_ptr<MaterialDescriptor> AssetManager::CreateMaterial(const std::string& materialPath)
{
    if (materialPath.empty()) return nullptr;

    std::shared_ptr<MaterialDescriptor> descriptor = std::make_shared<MaterialDescriptor>();
    descriptor->AssetId = m_FreeAssetId++;
    descriptor->AssetName = std::filesystem::path(materialPath).filename().string();
    descriptor->AssetType = AssetType::Material;

    descriptor->BaseColorTextureID = -1;
    descriptor->NormalTextureID = -1;
    descriptor->RoughnessTextureID = -1;
    descriptor->MetalicTextureID = -1;
    
    descriptor->BaseColor = glm::vec4(1.0f);
    descriptor->Roughness = 0.5f;
    descriptor->Metalic = 0.5f;

    std::string savePath = Files::GetSavePath(materialPath, AssetType::Material);
    
    SaveDescriptor(savePath, descriptor);
    
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
    
    AssetDescriptor* descriptorPtr;
    {
        AssetUtils::SetUseFullDescriptor(true);
        std::ifstream file(m_AssetIdToDescriptorPath[descriptor->AssetId], std::ios_base::binary);
        boost::archive::binary_iarchive ia(file);
        ia >> descriptorPtr;
    }
    descriptor->MeshData = std::move(static_cast<StaticMeshDescriptor*>(descriptorPtr)->MeshData);

    delete descriptorPtr;
    descriptorPtr = nullptr;
    
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
    
    AssetDescriptor* descriptorPtr;
    {
        AssetUtils::SetUseFullDescriptor(true);
        std::ifstream file(m_AssetIdToDescriptorPath[descriptor->AssetId], std::ios_base::binary);
        boost::archive::binary_iarchive ia(file);
        ia >> descriptorPtr;
    }

    descriptor->TextureData = std::move(static_cast<Texture2DDescriptor*>(descriptorPtr)->TextureData);
    
    delete descriptorPtr;
    descriptorPtr = nullptr;
    
    std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(descriptor);
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

    std::shared_ptr<Material> material = std::make_shared<Material>(); // TODO: Change shader to descriptor's one :)
    material->SetDescriptor(descriptor);
    m_Assets[descriptor->AssetId] = material;
    
    material->SetBaseColor(descriptor->BaseColor);
    material->SetRoughness(descriptor->Roughness);    
    material->SetMetalic(descriptor->Metalic);    

    material->SetBaseColorTexture(LoadTexture(descriptor->BaseColorTextureID));
    material->SetNormalTexture(LoadTexture(descriptor->NormalTextureID));
    material->SetRoughnessTexture(LoadTexture(descriptor->RoughnessTextureID));
    material->SetMetalicTexture(LoadTexture(descriptor->MetalicTextureID));

	ED_LOG(AssetManager, info, "Finished loading material: {}", descriptor->AssetName)
    return material;
}

std::shared_ptr<StaticMesh> AssetManager::LoadMesh(int32_t assetId)
{
    if (assetId == -1) return nullptr;
    return LoadMesh(std::static_pointer_cast<StaticMeshDescriptor>(m_Descriptors[assetId]));
}

std::shared_ptr<Texture2D> AssetManager::LoadTexture(int32_t assetId)
{
    if (assetId == -1) return nullptr;
    return LoadTexture(std::static_pointer_cast<Texture2DDescriptor>(m_Descriptors[assetId]));
}

std::shared_ptr<Material> AssetManager::LoadMaterial(int32_t assetId)
{
    if (assetId == -1) return nullptr;
    return LoadMaterial(std::static_pointer_cast<MaterialDescriptor>(m_Descriptors[assetId]));
}

const std::vector<std::shared_ptr<AssetDescriptor>>& AssetManager::GetDescriptors(AssetType type) const
{
    return m_DescriptorsByType.at(type);
}

const std::vector<std::shared_ptr<StaticMeshDescriptor>>& AssetManager::GetMeshDescriptors() const
{
    return *reinterpret_cast<const std::vector<std::shared_ptr<StaticMeshDescriptor>>*>(&m_DescriptorsByType.at(AssetType::StaticMesh));
}

const std::vector<std::shared_ptr<Texture2DDescriptor>>& AssetManager::GetTexture2DDescriptors() const
{
    return *reinterpret_cast<const std::vector<std::shared_ptr<Texture2DDescriptor>>*>(&m_DescriptorsByType.at(AssetType::Texture2D));
}

const std::vector<std::shared_ptr<MaterialDescriptor>>& AssetManager::GetMaterialDescriptors() const
{
    return *reinterpret_cast<const std::vector<std::shared_ptr<MaterialDescriptor>>*>(&m_DescriptorsByType.at(AssetType::Material));
}

std::shared_ptr<AssetDescriptor> AssetManager::GetAssetDescriptor(const std::string& path) const
{
    if (m_DescriptorPathToAssetId.count(path))
    {
        return m_Descriptors.at(m_DescriptorPathToAssetId.at(path));
    }
    return nullptr;
}

std::shared_ptr<AssetDescriptor> AssetManager::GetAssetDescriptor(int32_t assetId)
{
    return assetId == -1 ? nullptr : m_Descriptors[assetId];
}

void AssetManager::RefreshMaterial(const std::shared_ptr<MaterialDescriptor>& descriptor)
{
    if (m_Assets.count(descriptor->AssetId))
    {
        std::shared_ptr<Material> material = std::static_pointer_cast<Material>(m_Assets[descriptor->AssetId]);

        material->SetDescriptor(descriptor);

        material->SetBaseColor(descriptor->BaseColor);
        material->SetRoughness(descriptor->Roughness);    
        material->SetMetalic(descriptor->Metalic);    

        material->SetBaseColorTexture(LoadTexture(descriptor->BaseColorTextureID));
        material->SetNormalTexture(LoadTexture(descriptor->NormalTextureID));
        material->SetRoughnessTexture(LoadTexture(descriptor->RoughnessTextureID));
        material->SetMetalicTexture(LoadTexture(descriptor->MetalicTextureID));

        SaveDescriptor(m_AssetIdToDescriptorPath[descriptor->AssetId], descriptor);
    }
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

    ED_ASSERT(entry.exists() && !entry.is_directory(), "Couldn't find the scene: {}", path)

    {
        std::ifstream file(path, std::ios_base::binary);
        boost::archive::text_iarchive ia(file);
        ia >> *scene;
    }
    
    m_ScenesPaths.emplace_back(path, scene);
    
    ED_LOG(AssetManager, info, "Finished loading scene: {}", path)
    return scene;
}

bool AssetManager::IsAssetExtension(const std::string& extension)
{
    return extension == ".edmesh" || extension == ".edmaterial" || extension == ".edtexture";
}

void AssetManager::SaveDescriptor(const std::string& path, const std::shared_ptr<AssetDescriptor>& descriptor, bool bAddDescriptor)
{
    ED_LOG(AssetManager, info, "Started saving descriptor: {}", path)

    std::ofstream file(path, std::ios_base::binary);
    boost::archive::binary_oarchive oa(file);
    AssetUtils::SetUseFullDescriptor(true); // TODO: remove this ;)
    oa << descriptor.get();

    if (bAddDescriptor)
    {
        AddDescriptor(descriptor, path);
    }

	ED_LOG(AssetManager, info, "Finished saving descriptor: {}", path)
}
    
void AssetManager::SaveScene(const std::string& path, const std::shared_ptr<Scene>& scene)
{
	ED_LOG(AssetManager, info, "Started saving scene: {}", path)

    std::ofstream file(path, std::ios_base::binary);
    boost::archive::text_oarchive oa(file);
    oa << *scene;

	ED_LOG(AssetManager, info, "Finished saving scene: {}", path)
}
    
std::shared_ptr<AssetDescriptor> AssetManager::LoadDescriptor(const std::filesystem::path& filepath, bool loadFull)
{
    ED_LOG(AssetManager, info, "Started loading descriptor {}", filepath.string())

    std::string extension = filepath.extension().string();
    std::string path = filepath.string().c_str();
    
    AssetDescriptor* descriptor;
    {
        std::ifstream file(filepath.c_str(), std::ios_base::binary);
        boost::archive::binary_iarchive ia(file);
        AssetUtils::SetUseFullDescriptor(loadFull); // TODO: remove this ;)
        ia >> descriptor;
    }
    
    std::shared_ptr<AssetDescriptor> descriptorSharedPtr(descriptor);
    AddDescriptor(descriptorSharedPtr, path);

    m_FreeAssetId = std::max(m_FreeAssetId, descriptor->AssetId + 1);

	ED_LOG(AssetManager, info, "Finished loading descriptor {}", filepath.string())

    return descriptorSharedPtr;
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
        datas.push_back(ParseMesh(mesh, Transform(), materials, true));
    }
    
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        ParseMeshesSeparately(node->mChildren[i], scene, materials, datas);
    }
}

StaticSubmeshData AssetManager::ParseMesh(aiMesh* mesh, const Transform& transform, const std::vector<std::shared_ptr<MaterialDescriptor>>& materials, bool dropTranslation)
{
    StaticSubmeshData data;
    
    data.Vertices = new glm::vec3[mesh->mNumVertices];
    data.VerticesSize = mesh->mNumVertices * sizeof(glm::vec3);

    if (mesh->mColors[0])
    {
        data.Colors = new glm::vec4[mesh->mNumVertices];
        data.ColorsSize = mesh->mNumVertices * sizeof(glm::vec4);
    }
    if (mesh->mTextureCoords[0])
    {
        data.TextureCoordinates = new glm::vec3[mesh->mNumVertices];
        data.TextureCoordinatesSize = mesh->mNumVertices * sizeof(glm::vec3);
    }
    if (mesh->mNormals)
    {
        data.Normals = new glm::vec3[mesh->mNumVertices];
        data.NormalsSize = mesh->mNumVertices * sizeof(glm::vec3);
    }
    if (mesh->mTangents)
    {
        data.Tangents = new glm::vec3[mesh->mNumVertices];
        data.TangentsSize = mesh->mNumVertices * sizeof(glm::vec3);
    }
    if (mesh->mBitangents)
    {
        data.Bitangents = new glm::vec3[mesh->mNumVertices];
        data.BitangentsSize = mesh->mNumVertices * sizeof(glm::vec3);
    }
    if (mesh->mFaces)
    {
        data.Indices = new int32_t[3 * mesh->mNumFaces];
        data.IndicesSize = 3 * mesh->mNumFaces * sizeof(int32_t);
    }

    glm::mat4 world = transform.GetMatrix();
    glm::mat4 normal = transform.GetInversedTransposedMatrix();
    
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        aiVector3D& vertex = mesh->mVertices[i];
        data.Vertices[i] = world * glm::vec4(vertex.x, vertex.y, vertex.z, !dropTranslation);
        
        if (mesh->mColors[0]) {
            data.Colors[i] = *(glm::vec4*)&mesh->mColors[0][i];
        }

        if (mesh->mTextureCoords[0])
        {
            data.TextureCoordinates[i] = *(glm::vec3*)&mesh->mTextureCoords[0][i];
        }

        if (mesh->mNormals)
        {
            data.Normals[i] = normal * glm::vec4(*(glm::vec3*)&mesh->mNormals[i], 0.0f);
        }

        if (mesh->mTangents)
        {
            data.Tangents[i] = normal * glm::vec4(*(glm::vec3*)&mesh->mTangents[i], 0.0f);
        }

        if (mesh->mBitangents)
        {
            data.Bitangents[i] = normal * glm::vec4(*(glm::vec3*)&mesh->mBitangents[i], 0.0f);
        }
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
    {
        for (int32_t k = 0; k < 3; ++k)
        {
            data.Indices[i * 3 + k] = mesh->mFaces[i].mIndices[k];
        }
    }

    data.Name = mesh->mName.C_Str();
    
    if (mesh->mMaterialIndex < materials.size())
    {
        data.Material = materials[mesh->mMaterialIndex]->AssetId;
    }
    else
    {
        data.Material = -1;   
    }
    
    return data;
}
