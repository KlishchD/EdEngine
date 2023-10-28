#include "StaticMesh.h"
#include "Core/Ed.h"

BOOST_CLASS_EXPORT_IMPLEMENT(StaticMeshDescriptor)

StaticSubmeshData::~StaticSubmeshData()
{
    if (Vertices)
    {
        delete Vertices;
        Vertices = nullptr;
    }

    if (Colors)
    {
        delete Colors;
        Colors = nullptr;
    }
        
    if (TextureCoordinates)
    {
        delete TextureCoordinates;
        TextureCoordinates = nullptr;
    }

    if (Normals)
    {
        delete Normals;
        Normals = nullptr;
    }

    if (Tangents)
    {
        delete Tangents;
        Tangents = nullptr;
    }

    if (Bitangents)
    {
        delete Bitangents;
        Bitangents = nullptr;
    }

    if (Indices)
    {
        delete Indices;
        Indices = nullptr;
    }
}

StaticSubmeshData::StaticSubmeshData(StaticSubmeshData&& data) noexcept
{
    Name = std::move(data.Name);
    
    VerticesSize = data.VerticesSize;
    ColorsSize = data.ColorsSize;
    TextureCoordinatesSize = data.TextureCoordinatesSize;
    NormalsSize = data.NormalsSize;
    TangentsSize = data.TangentsSize;
    BitangentsSize = data.BitangentsSize;
    IndicesSize = data.IndicesSize;

    Vertices = data.Vertices;
    Colors = data.Colors;
    TextureCoordinates = data.TextureCoordinates;
    Normals = data.Normals;
    Tangents = data.Tangents;
    Bitangents = data.Bitangents;
    Indices = data.Indices;

    data.Vertices = nullptr;
    data.Colors = nullptr;
    data.TextureCoordinates = nullptr;
    data.Normals = nullptr;
    data.Tangents = nullptr;
    data.Bitangents = nullptr;
    data.Indices = nullptr;
    
    Material = data.Material;
}

StaticSubmeshData::StaticSubmeshData(): VerticesSize(0),
                                                      ColorsSize(0),
                                                      TextureCoordinatesSize(0),
                                                      NormalsSize(0),
                                                      TangentsSize(0),
                                                      BitangentsSize(0),
                                                      IndicesSize(0),
                                                      Vertices(nullptr),
                                                      Colors(nullptr),
                                                      TextureCoordinates(nullptr),
                                                      Normals(nullptr),
                                                      Tangents(nullptr),
                                                      Bitangents(nullptr),
                                                      Indices(nullptr),
                                                      Material(-1)
{
}

StaticSubmesh::StaticSubmesh(const std::string& name): m_Name(name)
{
}

StaticSubmesh::StaticSubmesh(const StaticSubmesh& submesh): m_Name(submesh.m_Name), m_VertexArray(submesh.m_VertexArray), m_Material(submesh.m_Material)
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

void StaticSubmesh::SetVertexArray(const std::shared_ptr<VertexArray>& vertexArray)
{
    m_VertexArray = vertexArray;
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
