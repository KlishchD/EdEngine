#include "Material.h"
#include "Core/Rendering/RenderingContex.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Engine.h"
#include "Core/Ed.h"
#include "Helpers/RenderingHelper.h"
#include "AssetManager.h"

Material::Material(const std::string& name) : Super(name)
{
}

AssetType Material::GetType() const
{
    return AssetType::Material;
}

void Material::SetBaseColor(glm::vec3 color)
{
	m_BaseColor = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
	MarkDirty();
}

void Material::SetRoughness(float roughness)
{
	m_Roughness = glm::clamp(roughness, 0.0f, 1.0f);
	MarkDirty();
}

void Material::SetMetalic(float metalic)
{
	m_Metalic = glm::clamp(metalic, 0.0f, 1.0f);
	MarkDirty();
}

void Material::SetEmission(float emission)
{
    m_Emission = emission;
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
    Super::Serialize(archive);

    archive & m_BaseColor;

	archive & m_Roughness;
	archive & m_Metalic;
	archive & m_Emission;
}

void Material::SerializeData(Archive& archive)
{
	Super::SerializeData(archive);

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

void Material::SetMetalicTexture(std::shared_ptr<Texture2D> texture)
{
    m_MetalicTexture = texture;
    MarkDirty();
}