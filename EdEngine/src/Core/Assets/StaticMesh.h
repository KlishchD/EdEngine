#pragma once

#include "Material.h"
#include "Rendering/VertexArray.h"
#include "Utils/AssetUtils.h"

struct StaticMeshImportParameters
{
    std::string MeshPath;
    
    bool JoinIdenticalVertices = true;
    bool GenUVCoords = true;
    bool CalculateTangentSpace = true;
    bool FixInfacingNormals = true;
    
    bool ImportAsOneMesh = true;
    bool ImportMaterials = true;
};

struct StaticSubmeshData
{
    ~StaticSubmeshData();
    StaticSubmeshData(StaticSubmeshData&& data) noexcept;
    StaticSubmeshData();

    std::string Name;
    
    int32_t VerticesSize;
    int32_t ColorsSize;
    int32_t TextureCoordinatesSize;
    int32_t NormalsSize;
    int32_t TangentsSize;
    int32_t BitangentsSize;
    int32_t IndicesSize;

    glm::vec3* Vertices;
    glm::vec4* Colors;
    glm::vec3* TextureCoordinates;
    glm::vec3* Normals;
    glm::vec3* Tangents;
    glm::vec3* Bitangents;
    int32_t* Indices;
    
    int32_t Material;
};

struct StaticMeshDescriptor: public AssetDescriptor
{
    StaticMeshImportParameters ImportParameters;
    std::vector<StaticSubmeshData> MeshData;
};

namespace boost
{
    namespace serialization
    {
        template <class Archive>
        void serialize(Archive& ar, StaticSubmeshData& data, uint32_t version)
        {
            ar & data.Name;

            ar & data.VerticesSize;
            ar & data.ColorsSize;
            ar & data.TextureCoordinatesSize;
            ar & data.NormalsSize;
            ar & data.TangentsSize;
            ar & data.BitangentsSize;
            ar & data.IndicesSize;

            if (data.VerticesSize)
            {
                if (!data.Vertices)
                {
                    data.Vertices = static_cast<glm::vec3*>(std::malloc(data.VerticesSize));
                }
                ar & make_binary_object(data.Vertices, data.VerticesSize);
            }
            if (data.ColorsSize)
            {
                if (!data.Colors)
                {
                    data.Colors = static_cast<glm::vec4*>(std::malloc(data.ColorsSize));
                }
                ar & make_binary_object(data.Colors, data.ColorsSize);
            }
            if (data.TextureCoordinatesSize)
            {
                if (!data.TextureCoordinates)
                {
                    data.TextureCoordinates = static_cast<glm::vec3*>(std::malloc(data.TextureCoordinatesSize));
                }
                ar & make_binary_object(data.TextureCoordinates, data.TextureCoordinatesSize);
            }
            if (data.NormalsSize)
            {
                if (!data.Normals)
                {
                    data.Normals = static_cast<glm::vec3*>(std::malloc(data.NormalsSize));
                }
                ar & make_binary_object(data.Normals, data.NormalsSize);
            }
            if (data.TangentsSize)
            {
                if (!data.Tangents)
                {
                    data.Tangents = static_cast<glm::vec3*>(std::malloc(data.TangentsSize));
                }
                ar & make_binary_object(data.Tangents, data.TangentsSize);
            }
            if (data.BitangentsSize)
            {
                if (!data.Bitangents)
                {
                    data.Bitangents = static_cast<glm::vec3*>(std::malloc(data.BitangentsSize));
                }
                ar & make_binary_object(data.Bitangents, data.BitangentsSize);
            }
            if (data.IndicesSize)
            {
                if (!data.Indices)
                {
                    data.Indices = static_cast<int32_t*>(std::malloc(data.IndicesSize));
                }
                ar & make_binary_object(data.Indices, data.IndicesSize);
            }
            
            
            ar & data.Material;
        }

        template <class Archive>
        void serialize(Archive& ar, StaticMeshImportParameters& parameters, uint32_t version)
        {
            ar & parameters.MeshPath;

            ar & parameters.JoinIdenticalVertices;
            ar & parameters.GenUVCoords;
            ar & parameters.CalculateTangentSpace;
            ar & parameters.FixInfacingNormals;
            ar & parameters.ImportAsOneMesh;
        }
        
        template <class Archive>
        void serialize(Archive& ar, StaticMeshDescriptor& descriptor, uint32_t version)
        {
            ar & boost::serialization::base_object<AssetDescriptor>(descriptor);

            ar & descriptor.ImportParameters;

            if (AssetUtils::UseFullDescriptor())
            {
                ar & descriptor.MeshData;
            }
        }
    }
}

class StaticSubmesh
{
public:
    StaticSubmesh() = default;
    StaticSubmesh(const std::string& name);
    StaticSubmesh(const StaticSubmesh& submesh);

    std::string GetName() const;
    void SetName(const std::string& name);
    
    void SetVertexArray(const std::shared_ptr<VertexArray>& vertexArray);
    void SetMaterial(const std::shared_ptr<Material>& material);
    
    std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }
    
private:
    std::string m_Name;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<Material> m_Material;
};

class StaticMesh: public Asset
{
public:
    StaticMesh() = default;
    StaticMesh(const StaticMesh& mesh);

    void AddSubmesh(const std::shared_ptr<StaticSubmesh>& submesh) { m_Submeshes.push_back(submesh); }
    const std::vector<std::shared_ptr<StaticSubmesh>>& GetSubmeshes() const { return m_Submeshes; }
private:
    std::vector<std::shared_ptr<StaticSubmesh>> m_Submeshes;
};

BOOST_CLASS_EXPORT_KEY(StaticMeshDescriptor)
BOOST_CLASS_VERSION(StaticMeshDescriptor, 1)