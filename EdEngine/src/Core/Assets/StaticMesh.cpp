#include "StaticMesh.h"
#include "Core/Ed.h"
#include "Core/Rendering/Buffers/VertexBuffer.h"
#include "Core/Rendering/Buffers/IndexBuffer.h"
#include "Utils/RenderingHelper.h"

StaticSubmesh::StaticSubmesh(const std::string& name) : Asset(name)
{
    SetImportParameters(std::make_shared<StaticMeshImportParameters>()); // TODO: remove with ObjectFactory
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
    Asset::SerializeData(archive);

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
    Asset::FreeData();

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

StaticMesh::StaticMesh(const std::string& name) : Asset(name)
{
    SetImportParameters(std::make_shared<StaticMeshImportParameters>()); // TODO: remove with ObjectFactory
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

void StaticMesh::AddSubmesh(std::shared_ptr<StaticSubmesh> submesh)
{
	m_Submeshes.push_back(submesh);
}

void StaticMesh::ResetState()
{
    
}

void StaticMesh::SerializeData(Archive& archive)
{
    Asset::SerializeData(archive);

    archive & m_Submeshes;
}

void StaticMesh::FreeData()
{
    Asset::FreeData();

    for (std::shared_ptr<StaticSubmesh> submesh : m_Submeshes)
    {
        submesh->FreeData();
    }
}
