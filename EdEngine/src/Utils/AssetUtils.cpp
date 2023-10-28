#include "AssetUtils.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/StaticMesh.h"

void AssetUtils::SetUseFullDescriptor(bool state)
{
    m_UseFullDescriptor = state;
}

bool AssetUtils::UseFullDescriptor()
{
    bool value = m_UseFullDescriptor;
    m_UseFullDescriptor = false;
    return value;
}

std::shared_ptr<StaticMesh> AssetUtils::CreateStaticMesh(const std::shared_ptr<StaticMeshDescriptor>& descriptor)
{
    std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>();

    mesh->SetDescriptor(descriptor);
    for (const StaticSubmeshData& data: descriptor->MeshData)
    {
        mesh->AddSubmesh(CreateStaticSubmesh(data));
    }

    return mesh;
}

std::shared_ptr<StaticMesh> AssetUtils::CreateStaticMesh(int32_t staticMeshId)
{
    return Engine::Get().GetManager<AssetManager>()->LoadMesh(staticMeshId);
}

std::shared_ptr<StaticSubmesh> AssetUtils::CreateStaticSubmesh(const StaticSubmeshData& data)
{
    std::shared_ptr<StaticSubmesh> submesh = std::make_shared<StaticSubmesh>(data.Name);
    
    std::shared_ptr<VertexArray> vao = std::make_shared<VertexArray>();

    vao->AddBuffer(
        data.Vertices,
        data.VerticesSize,
        VertexBufferLayout {{ "vertex", 1, ShaderDataType::Float3 }},
        0
    );

    if (data.Colors)
    {
        vao->AddBuffer(
            data.Colors,
            data.ColorsSize,
            VertexBufferLayout {{ "color", 1, ShaderDataType::Float4 }},
            1
        );
    }
    
    if (data.TextureCoordinates)
    {
        vao->AddBuffer(
            data.TextureCoordinates,
            data.TextureCoordinatesSize,
            VertexBufferLayout {{ "textureCoords", 1, ShaderDataType::Float3 }},
            2
        );
    }

    if (data.Normals)
    {
        vao->AddBuffer(
            data.Normals,
            data.NormalsSize,
            VertexBufferLayout {{ "normals", 1, ShaderDataType::Float3 }},
            3
        );
    }

    if (data.Tangents)
    {
        vao->AddBuffer(
            data.Tangents,
            data.TangentsSize,
            VertexBufferLayout {{ "tangent", 1, ShaderDataType::Float3 }},
            4
        );
    }
    
    if (data.Bitangents) // TODO: Use in shader ;)
        {
        vao->AddBuffer(
            data.Bitangents,
            data.BitangentsSize,
            VertexBufferLayout {{ "bitangent", 1, ShaderDataType::Float3 }},
            5
        );
        }

    vao->SetIndexBuffer(data.Indices, data.IndicesSize);

    std::shared_ptr<Material> material = Engine::Get().GetManager<AssetManager>()->LoadMaterial(data.Material);

    submesh->SetMaterial(material);
    submesh->SetVertexArray(vao);
    
    return submesh;
}
