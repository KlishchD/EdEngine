#include "Material.h"
#include "Core/Rendering/RenderingContex.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Engine.h"
#include "Core/Ed.h"
#include "Utils/RenderingHelper.h"
#include "AssetManager.h"

Material::Material(const std::string& name) : Asset(name)
{

}

AssetType Material::GetType() const
{
    return AssetType::Material;
}

void Material::SetBaseColor(glm::vec3 color)
{
	m_BaseColor = color;
	MarkDirty();
}

void Material::SetRoughness(float roughness)
{
	m_Roughness = roughness;
	MarkDirty();
}

void Material::SetMetalic(float metalic)
{
	m_Metalic = metalic;
	MarkDirty();
}

void Material::SetEmission(float emission)
{
    m_Emission = emission;
    MarkDirty();
}

void Material::SetMetalicTexture(std::shared_ptr<Texture2D> texture)
{
    m_MetalicTexture = texture;
    MarkDirty();
}

glm::vec3 Material::GetBaseColor() const
{
    return m_BaseColor;
}

float Material::GetRoughness() const
{
    return m_Roughness;
}

float Material::GetMetalic() const
{
    return m_Metalic;
}

float Material::GetEmission() const
{
    return m_Emission;
}

std::shared_ptr<Texture2D> Material::GetBaseColorTexture() const
{
    return m_BaseColorTexture;
}

std::shared_ptr<Texture2D> Material::GetNormalTexture() const
{
    return m_NormalTexture;
}

std::shared_ptr<Texture2D> Material::GetRoughnessTexture() const
{
    return m_RoughnessTexture;
}

std::shared_ptr<Texture2D> Material::GetMetalicTexture() const
{
    return m_MetalicTexture;
}

bool Material::ShouldPerformNormalMapping() const
{
    return m_NormalTexture != nullptr;
}

void Material::ResetState()
{
    
}

void Material::Serialize(Archive& archive)
{
    Asset::Serialize(archive);

    archive & m_BaseColor;

	archive & m_Roughness;
	archive & m_Metalic;
	archive & m_Emission;
}

void Material::SerializeData(Archive& archive)
{
	Asset::SerializeData(archive);

    m_BaseColorTexture = SerializationHelper::SerializeAsset(archive, m_BaseColorTexture);
    m_NormalTexture = SerializationHelper::SerializeAsset(archive, m_NormalTexture);
    m_RoughnessTexture = SerializationHelper::SerializeAsset(archive, m_RoughnessTexture);
    m_MetalicTexture = SerializationHelper::SerializeAsset(archive, m_MetalicTexture);
}

void Material::SetBaseColorTexture(std::shared_ptr<Texture2D> texture)
{
    m_BaseColorTexture = texture;
    MarkDirty();
}

void Material::SetNormalTexture(std::shared_ptr<Texture2D> texture)
{
    m_NormalTexture = texture;
    MarkDirty();
}

void Material::SetRoughnessTexture(std::shared_ptr<Texture2D> texture)
{
    m_RoughnessTexture = texture;
    MarkDirty();
}
