#pragma once

#include <filesystem>

#include "Core/Ed.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Asset.h"
#include "StaticMesh.h"
#include "Core/BaseManager.h"
#include "Core/Math/Transform.h"

struct Texture2DImportParameters;
struct DescriptorStaticMeshData;
struct StaticMeshImportParameters;
class VertexArray;
class Shader;
class Scene;
class Texture2D;
struct MaterialDescriptor;
struct Texture2DDescriptor;
struct StaticMeshDescriptor;
class Material;

class AssetManager: public BaseManager
{
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;
    
    void LoadDescriptors(const std::string& contentPath);
    
    std::vector<std::shared_ptr<StaticMeshDescriptor>> ImportMesh(const StaticMeshImportParameters& parameters);
    std::shared_ptr<Texture2DDescriptor> ImportTexture(const Texture2DImportParameters& parameters);
    std::vector<std::shared_ptr<MaterialDescriptor>> ImportMaterial(const std::string& materialPath);
    
    std::shared_ptr<MaterialDescriptor> CreateMaterial(const std::string& materialPath);
    
    std::shared_ptr<StaticMesh> LoadMesh(const std::shared_ptr<StaticMeshDescriptor>& descriptor);
    std::shared_ptr<Texture2D> LoadTexture(const std::shared_ptr<Texture2DDescriptor>& descriptor);
    std::shared_ptr<Texture2D> LoadTexture(const Texture2DImportParameters& parameters);
    std::shared_ptr<Material> LoadMaterial(const std::shared_ptr<MaterialDescriptor>& descriptor);

    std::shared_ptr<StaticMesh> LoadMesh(int32_t assetId);
    std::shared_ptr<Texture2D> LoadTexture(int32_t assetId);
    std::shared_ptr<Material> LoadMaterial(int32_t assetId);

    const std::vector<std::shared_ptr<AssetDescriptor>>& GetDescriptors(AssetType type) const;
    const std::vector<std::shared_ptr<StaticMeshDescriptor>>& GetMeshDescriptors() const;
    const std::vector<std::shared_ptr<Texture2DDescriptor>>& GetTexture2DDescriptors() const;
    const std::vector<std::shared_ptr<MaterialDescriptor>>& GetMaterialDescriptors() const;

    std::shared_ptr<AssetDescriptor> GetAssetDescriptor(const std::string& path) const;
    std::shared_ptr<AssetDescriptor> GetAssetDescriptor(int32_t assetId);
    
    void RefreshMaterial(const std::shared_ptr<MaterialDescriptor>& descriptor);

    std::shared_ptr<Scene> CreateOrLoadScene(const std::string& path);

    bool IsAssetExtension(const std::string& extension);
private:
    Assimp::Importer m_Importer;

    int32_t m_FreeAssetId = 1; // TODO: Make it some GUID or similar ;)
    std::map<int32_t, std::shared_ptr<AssetDescriptor>> m_Descriptors;
    std::map<int32_t, std::shared_ptr<Asset>> m_Assets;
    
    std::map<AssetType, std::vector<std::shared_ptr<AssetDescriptor>>> m_DescriptorsByType;
    
    std::map<int32_t, std::string> m_AssetIdToDescriptorPath;
    std::map<std::string, int32_t> m_DescriptorPathToAssetId;

    std::vector<std::pair<std::string, std::shared_ptr<Scene>>> m_ScenesPaths;

    std::vector<std::shared_ptr<MaterialDescriptor>> ImportMaterialInternal(const aiScene* scene, const std::string& materialPath);
    
    void SaveDescriptor(const std::string& path, const std::shared_ptr<AssetDescriptor>& descriptor, bool bAddDescriptor = true);
    std::shared_ptr<AssetDescriptor> LoadDescriptor(const std::filesystem::path& filepath, bool loadFull);
    
    void AddDescriptor(const std::shared_ptr<AssetDescriptor>& descriptor, const std::string& path);
    
    void SaveScene(const std::string& path, const std::shared_ptr<Scene>& scene);

    void ParseNodesAndCombineInOneMesh(aiNode* node, const aiScene* scene, const Transform& parentTransform, const std::vector<std::shared_ptr<MaterialDescriptor>>& materials, std::vector<StaticSubmeshData>& datas);
    void ParseMeshesSeparately(aiNode* node, const aiScene* scene, const std::vector<std::shared_ptr<MaterialDescriptor>>& materials, std::vector<StaticSubmeshData>& datas);
    StaticSubmeshData ParseMesh(aiMesh* mesh, const Transform& transform, const std::vector<std::shared_ptr<MaterialDescriptor>>& materials, bool dropTranslation);
    Transform ParseSubmeshTransformation(aiNode* node);
    int32_t GetImportParametersIntegerRepresentation(const StaticMeshImportParameters& parameters);
};
