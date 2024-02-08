#pragma once

#include "Asset.h"
#include "Descriptors/MaterialDescriptor.h"

class Texture2D;
class RenderingContext;

class Material : public Asset
{
public:
    Material(const Material& material);
    Material();

    virtual void SetDescriptor(std::shared_ptr<AssetDescriptor> inDescriptor);
    virtual void SyncDescriptor() override;

    void SetShaderData(std::shared_ptr<RenderingContext> context);
    
    void SetBaseColor(glm::vec3 color) { m_BaseColor = color; }
    void SetRoughness(float roughness) { m_Roughness = roughness; }
    void SetMetalic(float metalic) { m_Metalic = metalic; }
    void SetEmission(float emission);

    void SetBaseColorTexture(std::shared_ptr<Texture2D> texture);
    void SetNormalTexture(std::shared_ptr<Texture2D> texture);
    void SetRoughnessTexture(std::shared_ptr<Texture2D> texture);
    void SetMetalicTexture(std::shared_ptr<Texture2D> texture);

    glm::vec3 GetBaseColor() const;
    float GetRoughness() const;
    float GetMetalic() const;
    float GetEmission() const;

    std::shared_ptr<Texture2D> GetBaseColorTexture() const;
    std::shared_ptr<Texture2D> GetNormalTexture() const;
    std::shared_ptr<Texture2D> GetRoughnessTexture() const;
    std::shared_ptr<Texture2D> GetMetalicTexture() const;
private:

    std::shared_ptr<Texture2D> m_BaseColorTexture;
    std::shared_ptr<Texture2D> m_NormalTexture;
    
    std::shared_ptr<Texture2D> m_RoughnessTexture;
    std::shared_ptr<Texture2D> m_MetalicTexture;
    
    glm::vec3 m_BaseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    float m_Roughness = 1.0f;
    float m_Metalic = 1.0f;
    float m_Emission = 0.0f;
};
