#include "StaticMesh.h"
#include "Core/Ed.h"

BOOST_CLASS_EXPORT_IMPLEMENT(StaticMeshDescriptor)

StaticSubmeshData::StaticSubmeshData(StaticSubmeshData&& data) noexcept
{
    Name = std::move(data.Name);
	Vertices = std::move(data.Vertices);
    Indices = std::move(data.Indices);

    Material = data.Material;
}

StaticSubmeshData::StaticSubmeshData()
{
}

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

void StaticSubmesh::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer)
{
    m_VertexBuffer = buffer;
}

void StaticSubmesh::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer)
{
	m_IndexBuffer = buffer;
}

void StaticSubmesh::SetMaterial(const std::shared_ptr<Material>& material)
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
}
