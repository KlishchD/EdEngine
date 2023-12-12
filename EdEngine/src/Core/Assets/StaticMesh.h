#pragma once

#include "Material.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include "Utils/SerializationUtils.h"
#include "Utils/AssetUtils.h"

class VertexBuffer;
class IndexBuffer;

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

struct Vertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec3 TextureCoordinates;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct StaticSubmeshData
{
    StaticSubmeshData(StaticSubmeshData&& data) noexcept;
    StaticSubmeshData();

    std::string Name;
    
    std::vector<Vertex> Vertices;
    std::vector<int32_t> Indices;
    
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
		void serialize(Archive& ar, Vertex& vertex, uint32_t version)
		{
			ar & vertex.Position;
			ar & vertex.Color;
			ar & vertex.TextureCoordinates;
			ar & vertex.Normal;
			ar & vertex.Tangent;
			ar & vertex.Bitangent;
		}

        template <class Archive>
        void serialize(Archive& ar, StaticSubmeshData& data, uint32_t version)
        {
            ar & data.Name;
            ar & data.Vertices;
            ar & data.Indices;
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

class VertexBuffer;

class StaticSubmesh
{
public:
    StaticSubmesh() = default;
    StaticSubmesh(const std::string& name);
    StaticSubmesh(const StaticSubmesh& submesh);

    std::string GetName() const;
    void SetName(const std::string& name);
    
    void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer);
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer);
    void SetMaterial(const std::shared_ptr<Material>& material);
    
    std::shared_ptr<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
    std::shared_ptr<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }
    
private:
    std::string m_Name;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
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