#include "StaticMesh.h"
#include "Core/Ed.h"
#include "Core/Rendering/Buffers/VertexBuffer.h"
#include "Core/Rendering/Buffers/IndexBuffer.h"
#include "Utils/RenderingHelper.h"

StaticSubmesh::StaticSubmesh(const std::string& name) : Asset(name)
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

void StaticSubmesh::Serialize(Archive& archive)
{
	if (archive.GetMode() == ArchiveMode::Write)
	{
		archive & GetType();
	}

	Serializable::Serialize(archive);

	archive & m_Id;
	archive & m_Name;
}

void StaticSubmesh::SerializeData(Archive& archive)
{
    Asset::SerializeData(archive);

    m_Material = SerializationHelper::SerializeAsset(archive, m_Material);

    archive & m_Vertices;
    archive & m_Indices;
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

	m_VertexBuffer = RenderingHelper::CreateVertexBuffer((void*)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex), layout, BufferUsage::StaticDraw);
	m_IndexBuffer = RenderingHelper::CreateIndexBuffer((void*)m_Indices.data(), m_Indices.size() * sizeof(int32_t), BufferUsage::StaticDraw);
}

StaticMesh::StaticMesh(const std::string& name) : Asset(name)
{

}

AssetType StaticMesh::GetType() const
{
    return AssetType::StaticMesh;
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
