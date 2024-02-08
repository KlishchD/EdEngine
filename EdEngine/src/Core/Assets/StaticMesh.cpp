#include "StaticMesh.h"
#include "Core/Ed.h"

StaticSubmesh::StaticSubmesh(const std::string& name): m_Name(name)
{
}

StaticSubmesh::StaticSubmesh(const StaticSubmesh& submesh): m_Name(submesh.m_Name), m_VertexBuffer(submesh.m_VertexBuffer), m_Material(submesh.m_Material)
{
}

std::string StaticSubmesh::GetName() const
{
    return m_Name;
}

void StaticSubmesh::SetName(const std::string& name)
{
    m_Name = name;
}

void StaticSubmesh::SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer)
{
    m_VertexBuffer = buffer;
}

void StaticSubmesh::SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer)
{
	m_IndexBuffer = buffer;
}

void StaticSubmesh::SetMaterial(std::shared_ptr<Material> material)
{
    m_Material = material;
}

StaticMesh::StaticMesh(const StaticMesh& mesh)
{
    SetDescriptor(mesh.GetDescriptor());
    
    for (const std::shared_ptr<StaticSubmesh> submesh: mesh.m_Submeshes)
    {
        m_Submeshes.push_back(std::make_shared<StaticSubmesh>(*submesh));
    }

    MarkDirty();
}

void StaticMesh::SetDescriptor(std::shared_ptr<AssetDescriptor> inDescriptor)
{
    m_Descriptor = inDescriptor;

    std::shared_ptr<StaticMeshDescriptor> descriptor = std::static_pointer_cast<StaticMeshDescriptor>(inDescriptor);
    for (const StaticSubmeshData& data : descriptor->MeshData)
    {
        m_Submeshes.push_back(AssetUtils::CreateStaticSubmesh(data));
    }
}

void StaticMesh::SyncDescriptor()
{
    Asset::SyncDescriptor();

    std::shared_ptr<StaticMeshDescriptor> descriptor = GetDescriptor<StaticMeshDescriptor>();

    for (int32_t i = 0; i < m_Submeshes.size(); ++i)
    {
        descriptor->MeshData[i].MaterialID = m_Submeshes[i]->GetMaterial()->GetDescriptor()->AssetId;
    }
}
