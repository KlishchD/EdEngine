#pragma once

#include "Asset.h"
#include "Material.h"
#include "Descriptors/StaticMeshDescriptor.h"

class VertexBuffer;
class IndexBuffer;
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

	virtual void SyncDescriptor() override;

    void AddSubmesh(const std::shared_ptr<StaticSubmesh>& submesh) { m_Submeshes.push_back(submesh); }
    const std::vector<std::shared_ptr<StaticSubmesh>>& GetSubmeshes() const { return m_Submeshes; }
private:
    std::vector<std::shared_ptr<StaticSubmesh>> m_Submeshes;
};