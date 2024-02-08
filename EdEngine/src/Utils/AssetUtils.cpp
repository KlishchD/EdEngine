#include "AssetUtils.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/StaticMesh.h"
#include "Core/Rendering/Types.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Buffers/VertexBufferLayout.h"
#include "RenderingHelper.h"
#include "Core/Macros.h"

std::shared_ptr<Material> AssetUtils::CreateMaterial(const std::shared_ptr<MaterialDescriptor>& descriptor)
{
	std::shared_ptr<Material> material = std::make_shared<Material>();
	material->SetDescriptor(descriptor);
    return material;
}

std::shared_ptr<StaticMesh> AssetUtils::CreateStaticMesh(const std::shared_ptr<StaticMeshDescriptor>& descriptor)
{
    std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>();
    mesh->SetDescriptor(descriptor);
    return mesh;
}

std::shared_ptr<StaticMesh> AssetUtils::CreateStaticMesh(UUID staticMeshId)
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

    std::shared_ptr<Material> material = Engine::Get().GetManager<AssetManager>()->LoadMaterial(data.MaterialID);

    submesh->SetMaterial(material);
    submesh->SetVertexBuffer(vbo);
    submesh->SetIndexBuffer(ibo);
    
    return submesh;
}

AssetType AssetUtils::GetAssetTypeFromExtension(const std::string& extension)
{
	if (extension == ".edmesh")
	{
		return AssetType::StaticMesh;
	}
	else if (extension == ".edmaterial")
	{
		return AssetType::Material;
	}
	else if (extension == ".edtexture")
	{
		return AssetType::Texture2D;
	}

    ED_ASSERT(0, "Unknown extension")
}

bool AssetUtils::IsAssetExtension(const std::string& extension)
{
	return extension == ".edmesh" || extension == ".edmaterial" || extension == ".edtexture";
}

std::string AssetUtils::GetSelectTextureLable(const std::shared_ptr<Texture2D>& texture)
{
	return texture ? texture->GetAssetName() : "None";
}

std::string AssetUtils::GetAssetNameLable(const std::shared_ptr<AssetDescriptor>& descriptor)
{
	return descriptor->AssetName + "##" + std::to_string((int32_t)descriptor.get());
}
