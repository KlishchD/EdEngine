#include "Material.h"
#include "Core/Rendering/RenderingContex.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Assets/Descriptors/MaterialDescriptor.h"
#include "Core/Ed.h"
#include "Utils/RenderingHelper.h"

Material::Material(const Material& material) : m_BaseColorTexture(material.m_BaseColorTexture),
    m_NormalTexture(material.m_NormalTexture), m_RoughnessTexture(material.m_RoughnessTexture), m_MetalicTexture(material.m_MetalicTexture),
    m_BaseColor(material.m_BaseColor), m_Roughness(material.m_Roughness), m_Metalic(material.m_Metalic), m_Emission(material.m_Emission)
{
    
}

Material::Material()
{
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

void Material::SetShaderData(const std::shared_ptr<RenderingContext>& context)
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
}

void Material::SetMetalicTexture(const std::shared_ptr<Texture2D>& texture)
{
    m_MetalicTexture = texture;
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

void Material::SetBaseColorTexture(const std::shared_ptr<Texture2D>& texture)
{
    m_BaseColorTexture = texture;
}

void Material::SetNormalTexture(const std::shared_ptr<Texture2D>& texture)
{
    m_NormalTexture = texture;
}

void Material::SetRoughnessTexture(const std::shared_ptr<Texture2D>& texture)
{
    m_RoughnessTexture = texture;
}
