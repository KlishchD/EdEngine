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

	void Serialize(Archive& ar);
};

ED_CLASS(StaticSubmesh) : public Asset
{
	ED_CLASS_BODY(StaticSubmesh, Asset)
public:
	StaticSubmesh(const std::string& name = "Empty");
	
	virtual AssetType GetType() const override;
	
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

ED_CLASS(StaticMesh): public Asset
{
	ED_CLASS_BODY(StaticMesh, Asset)
public:
	StaticMesh(const std::string& name = "Empty");
	
	virtual AssetType GetType() const override;
	
	virtual void SetShouldLoadData(bool status) override;
	
	void SetSubmeshes(const std::vector<std::shared_ptr<StaticSubmesh>>& submeshes);
	void SetSubmeshes(std::vector<std::shared_ptr<StaticSubmesh>>&& submeshes);
	void AddSubmesh(std::shared_ptr<StaticSubmesh> submesh);
	const std::vector<std::shared_ptr<StaticSubmesh>>& GetSubmeshes() const { return m_Submeshes; }
	
	virtual void ResetState() override;
	
	virtual void Serialize(Archive& archive) override;
	virtual void SerializeData(Archive& archive) override;
	virtual void FreeData() override;
private:
    std::vector<std::shared_ptr<StaticSubmesh>> m_Submeshes;
};