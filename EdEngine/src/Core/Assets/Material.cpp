#include "Material.h"
#include "Core/Rendering/RenderingContex.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Assets/Descriptors/MaterialDescriptor.h"
#include "Core/Engine.h"
#include "Core/Ed.h"
#include "Utils/RenderingHelper.h"
#include "AssetManager.h"

Material::Material(const Material& material) : m_BaseColorTexture(material.m_BaseColorTexture),
    m_NormalTexture(material.m_NormalTexture), m_RoughnessTexture(material.m_RoughnessTexture), m_MetalicTexture(material.m_MetalicTexture),
    m_BaseColor(material.m_BaseColor), m_Roughness(material.m_Roughness), m_Metalic(material.m_Metalic), m_Emission(material.m_Emission)
{
    
}

Material::Material()
{
}

void Material::SetDescriptor(std::shared_ptr<AssetDescriptor> inDescriptor)
{
    m_Descriptor = inDescriptor;

    std::shared_ptr<MaterialDescriptor> descriptor = std::static_pointer_cast<MaterialDescriptor>(inDescriptor);

    m_BaseColor = descriptor->BaseColor;
    m_Metalic = descriptor->Metalic;
    m_Roughness = descriptor->Roughness;
    m_Emission = descriptor->Emission;

    std::shared_ptr<AssetManager> manager = Engine::Get().GetManager<AssetManager>();

    m_BaseColorTexture = manager->LoadTexture(descriptor->BaseColorTextureID);
    m_MetalicTexture = manager->LoadTexture(descriptor->MetalicTextureID);
    m_RoughnessTexture = manager->LoadTexture(descriptor->RoughnessTextureID);
    m_NormalTexture = manager->LoadTexture(descriptor->NormalTextureID);
}

void Material::SyncDescriptor()
{
    std::shared_ptr<MaterialDescriptor> descritor = GetDescriptor<MaterialDescriptor>();

    descritor->BaseColor = m_BaseColor;
    descritor->Roughness = m_Roughness;
    descritor->Metalic = m_Metalic;
    descritor->Emission = m_Emission;

    descritor->BaseColorTextureID = m_BaseColorTexture ? m_BaseColorTexture->GetDescriptor()->AssetId : UUIDs::nil_uuid();
    descritor->NormalTextureID = m_NormalTexture ? m_NormalTexture->GetDescriptor()->AssetId : UUIDs::nil_uuid();
    descritor->RoughnessTextureID = m_RoughnessTexture ? m_RoughnessTexture->GetDescriptor()->AssetId : UUIDs::nil_uuid();
    descritor->MetalicTextureID = m_MetalicTexture ? m_MetalicTexture->GetDescriptor()->AssetId : UUIDs::nil_uuid();
}

void Material::SetShaderData(std::shared_ptr<RenderingContext> context)
{
    context->SetShaderDataFloat3("u_Material.BaseColor", m_BaseColor);
    
    context->SetShaderDataFloat("u_Material.Roughness", m_Roughness);
    context->SetShaderDataFloat("u_Material.Metalic", m_Metalic);
    context->SetShaderDataFloat("u_Material.Emission", m_Emission);
    
    context->SetShaderDataInt("u_Material.NormalTexture", 1);

    std::shared_ptr<Texture2D> white = RenderingHelper::GetWhiteTexture();
    
    context->SetShaderDataTexture("u_Material.BaseColorTexture", m_BaseColorTexture ? m_BaseColorTexture : white);

    context->SetShaderDataTexture("u_Material.NormalTexture", m_NormalTexture ? m_NormalTexture : white);
    context->SetShaderDataBool("u_Material.HasNormalTexture", m_NormalTexture != nullptr);
    context->SetShaderDataBool("u_PerformNormalMapping", m_NormalTexture != nullptr);

    context->SetShaderDataTexture("u_Material.RoughnessTexture", m_RoughnessTexture ? m_RoughnessTexture : white);
    context->SetShaderDataTexture("u_Material.MetalicTexture", m_MetalicTexture ? m_MetalicTexture : white);
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
