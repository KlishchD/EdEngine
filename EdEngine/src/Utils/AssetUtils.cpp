#include "AssetUtils.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/StaticMesh.h"
#include "Core/Rendering/Types.h"
#include "Core/Rendering/Buffers/VertexBufferLayout.h"
#include "RenderingHelper.h"

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
    
    VertexBufferLayout layout = {
        { "Position",            ShaderDataType::Float3 },
        { "Color",               ShaderDataType::Float4 },
        { "TextureCoordinates",  ShaderDataType::Float3 },
        { "Normal",              ShaderDataType::Float3 },
        { "Tangent",             ShaderDataType::Float3 },
        { "Bitangent",           ShaderDataType::Float3 }
    };

    std::shared_ptr<VertexBuffer> vbo = RenderingHelper::CreateVertexBuffer((void*) data.Vertices.data(), data.Vertices.size() * sizeof(Vertex), layout, BufferUsage::StaticDraw);
    std::shared_ptr<IndexBuffer> ibo = RenderingHelper::CreateIndexBuffer((void*)data.Indices.data(), data.Indices.size() * sizeof(int32_t), BufferUsage::StaticDraw);

    std::shared_ptr<Material> material = Engine::Get().GetManager<AssetManager>()->LoadMaterial(data.Material);

    submesh->SetMaterial(material);
    submesh->SetVertexBuffer(vbo);
    submesh->SetIndexBuffer(ibo);
    
    return submesh;
}
