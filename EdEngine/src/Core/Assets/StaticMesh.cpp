#include "StaticMesh.h"
#include "Core/Ed.h"
#include "Core/Rendering/Buffers/VertexBuffer.h"
#include "Core/Rendering/Buffers/IndexBuffer.h"
#include "Helpers/RenderingHelper.h"

StaticSubmesh::StaticSubmesh(const std::string& name) : Super(name)
{
}

AssetType StaticSubmesh::GetType() const
{
    return AssetType::StaticSubmesh;
}

void StaticSubmesh::SetData(const std::vector<Vertex>& vertices, const std::vector<int32_t>& indices)
{
    m_Vertices = vertices;
    m_Indices = indices;
    CreateBuffers();
}

void StaticSubmesh::SetData(std::vector<Vertex>&& vertices, std::vector<int32_t>&& indices)
{
	m_Vertices = std::move(vertices);
	m_Indices = std::move(indices);
	CreateBuffers();
}

void StaticSubmesh::SetMaterial(std::shared_ptr<Material> material)
{
    m_Material = material;
}

void StaticSubmesh::ResetState()
{

}

void StaticSubmesh::SerializeData(Archive& archive)
{
    Super::SerializeData(archive);

    m_Material = SerializationHelper::SerializeAsset(archive, m_Material);

    archive & m_Vertices;
    archive & m_Indices;

    if (archive.GetMode() == ArchiveMode::Read)
    {
        CreateBuffers();
    }
}

void StaticSubmesh::FreeData()
{
    Super::FreeData();

    m_Indices.clear();
    m_Vertices.clear();
}

void StaticSubmesh::CreateBuffers()
{
    static VertexBufferLayout layout = {
    		{ "Position",            ShaderDataType::Float3 },
    		{ "Color",               ShaderDataType::Float4 },
    		{ "TextureCoordinates",  ShaderDataType::Float3 },
    		{ "Normal",              ShaderDataType::Float3 },
    		{ "Tangent",             ShaderDataType::Float3 },
    		{ "Bitangent",           ShaderDataType::Float3 }
    };
    
    if (m_VertexBuffer)
    {
        m_VertexBuffer->SetData((void*)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex), BufferUsage::StaticDraw);
    }
    else
    {
        m_VertexBuffer = RenderingHelper::CreateVertexBuffer((void*)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex), layout, BufferUsage::StaticDraw);
    }
    
    if (m_IndexBuffer)
    {
        m_IndexBuffer->SetData((void*)m_Indices.data(), m_Indices.size() * sizeof(int32_t), BufferUsage::StaticDraw);
    }
    else
    {
        m_IndexBuffer = RenderingHelper::CreateIndexBuffer((void*)m_Indices.data(), m_Indices.size() * sizeof(int32_t), BufferUsage::StaticDraw);
    }
}

void StaticSubmesh::Serialize(Archive& archive)
{
    archive & m_Name;
}

StaticMesh::StaticMesh(const std::string& name) : Asset(name)
{
}

AssetType StaticMesh::GetType() const
{
    return AssetType::StaticMesh;
}

void StaticMesh::SetShouldLoadData(bool status)
{
    m_bShouldHaveData = status;

    for (const std::shared_ptr<StaticSubmesh>& submesh : m_Submeshes)
    {
        submesh->SetShouldLoadData(status);
    }
}

void StaticMesh::SetSubmeshes(const std::vector<std::shared_ptr<StaticSubmesh>>& submeshes)
{
    m_Submeshes = submeshes;
}

void StaticMesh::SetSubmeshes(std::vector<std::shared_ptr<StaticSubmesh>>&& submeshes)
{
    m_Submeshes = std::move(submeshes);
}

void StaticMesh::AddSubmesh(std::shared_ptr<StaticSubmesh> submesh)
{
	m_Submeshes.push_back(submesh);
}

void StaticMesh::ResetState()
{
    
}

void StaticMesh::FreeData()
{
    Super::FreeData();

    for (std::shared_ptr<StaticSubmesh> submesh : m_Submeshes)
    {
        submesh->FreeData();
    }
}

void StaticMesh::Serialize(Archive& archive)
{
    Super::Serialize(archive);

    archive & m_Submeshes;
}

void StaticMesh::SerializeData(Archive& archive)
{
    Super::SerializeData(archive);

    for (std::shared_ptr<StaticSubmesh> submesh : m_Submeshes)
    {
        submesh->SerializeData(archive);
    }
}

void Vertex::Serialize(Archive& ar)
{
    ar & Position;
    ar & Color;
    ar & TextureCoordinates;
    ar & Normal;
    ar & Tangent;
    ar & Bitangent;
}
