#pragma once

#include "Asset.h"
#include "ImportParameters/StaticMeshImportParameters.h"
#include "Material.h"
#include <glm/vec4.hpp>

class VertexBuffer;
class IndexBuffer;
class VertexBuffer;

struct Vertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec3 TextureCoordinates;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
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
	}
}

class StaticSubmesh : public Asset
{
public:
	StaticSubmesh(const std::string& name = "Empty");

	virtual AssetType GetType() const;

    void SetData(const std::vector<Vertex>& vertices, const std::vector<int32_t>& indices);
    void SetData(std::vector<Vertex>&& vertices, std::vector<int32_t>&& indices);

    void SetMaterial(std::shared_ptr<Material> material);
    
    std::shared_ptr<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
    std::shared_ptr<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }

	virtual void ResetState() override;

    virtual void Serialize(Archive& archive) override;
    virtual void SerializeData(Archive& archive) override;
	virtual void FreeData() override;

protected:
    void CreateBuffers();

protected:
	std::shared_ptr<Material> m_Material;
	
    std::vector<Vertex> m_Vertices;
	std::vector<int32_t> m_Indices;

    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

class StaticMesh: public Asset
{
public:
	typedef ImportParametersClass StaticMeshImportParameters;

	StaticMesh(const std::string& name = "Empty");

	virtual AssetType GetType() const;

    void SetSubmeshes(const std::vector<std::shared_ptr<StaticSubmesh>>& submeshes);
    void AddSubmesh(std::shared_ptr<StaticSubmesh> submesh);
    const std::vector<std::shared_ptr<StaticSubmesh>>& GetSubmeshes() const { return m_Submeshes; }

	virtual void ResetState() override;

    virtual void SerializeData(Archive& archive) override;
    virtual void FreeData() override;
private:
    std::vector<std::shared_ptr<StaticSubmesh>> m_Submeshes;
};