#include "Material.h"

#include "Texture2D.h"
#include "Core/Ed.h"

BOOST_CLASS_EXPORT_IMPLEMENT(MaterialDescriptor)

Material::Material(const Material& material) : m_BaseColorTexture(material.m_BaseColorTexture),
    m_NormalTexture(material.m_NormalTexture), m_RoughnessTexture(material.m_RoughnessTexture), m_MetalicTexture(material.m_MetalicTexture),
    m_BaseColor(material.m_BaseColor), m_Roughness(material.m_Roughness), m_Metalic(material.m_Metalic)
{
    
}

Material::Material()
{
}

void Material::SetShaderData(const std::shared_ptr<Shader>& shared)
{
    shared->SetFloat4("u_Material.BaseColor", m_BaseColor);
    
    shared->SetFloat("u_Material.Roughness", m_Roughness);
    shared->SetFloat("u_Material.Metalic", m_Metalic);
    
    shared->SetInt("u_Material.NormalTexture", 1);

    Texture2D::GetWhiteTexture()->Bind(0);

    if (m_BaseColorTexture)
    {
        m_BaseColorTexture->Bind(1);
        shared->SetInt("u_Material.BaseColorTexture", 1);
    }
    else
    {
        shared->SetInt("u_Material.BaseColorTexture", 0);
    }
    
    if (m_NormalTexture)
    {
        m_NormalTexture->Bind(2);
        shared->SetInt("u_Material.NormalTexture", 2);
        shared->SetBool("u_Material.HasNormalTexture", true);
        shared->SetBool("u_PerformNormalMapping", true);
    }
    else
    {
        shared->SetInt("u_Material.NormalTexture", 0);
        shared->SetBool("u_Material.HasNormalTexture", false);
        shared->SetBool("u_PerformNormalMapping", false);
    }

    if (m_RoughnessTexture)
    {
        m_RoughnessTexture->Bind(3);
        shared->SetInt("u_Material.RoughnessTexture", 3);
    }
    else
    {
        shared->SetInt("u_Material.RoughnessTexture", 0);
    }

    if (m_MetalicTexture)
    {
        m_MetalicTexture->Bind(4);
        shared->SetInt("u_Material.MetalicTexture", 4);
    }
    else
    {
        shared->SetInt("u_Material.MetalicTexture", 0);
    }
}

void Material::SetMetalicTexture(const std::shared_ptr<Texture2D>& texture)
{
    m_MetalicTexture = texture;
    if (texture) {
        m_Metalic = 1.0f;
    }
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
    if (texture) {
        m_Roughness = 1.0f;
    }
}
