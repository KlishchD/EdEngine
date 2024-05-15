#pragma once

#include "Asset.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "ImportParameters/MaterialImportParameters.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

ED_CLASS(Material) : public Asset
{
    ED_CLASS_BODY(Material, Asset)
public:
    Material(const std::string& name = "Empty");

	virtual AssetType GetType() const override;

    void SetBaseColor(glm::vec3 color);
    void SetRoughness(float roughness);
    void SetMetalic(float metalic);
    void SetEmission(float emission);

    glm::vec3 GetBaseColor() const;
    float GetRoughness() const;
    float GetMetalic() const;
    float GetEmission() const;

	void SetBaseColorTexture(std::shared_ptr<Texture2D> texture);
	void SetNormalTexture(std::shared_ptr<Texture2D> texture);
	void SetRoughnessTexture(std::shared_ptr<Texture2D> texture);
	void SetMetalicTexture(std::shared_ptr<Texture2D> texture);

    std::shared_ptr<Texture2D> GetBaseColorTexture() const;
    std::shared_ptr<Texture2D> GetNormalTexture() const;
    std::shared_ptr<Texture2D> GetRoughnessTexture() const;
    std::shared_ptr<Texture2D> GetMetalicTexture() const;

    bool ShouldPerformNormalMapping() const;

    virtual void ResetState() override;

    virtual void Serialize(Archive& archive) override;
    virtual void SerializeData(Archive& archive) override;

protected:
    std::shared_ptr<Texture2D> m_BaseColorTexture;
    std::shared_ptr<Texture2D> m_NormalTexture;
    
    std::shared_ptr<Texture2D> m_RoughnessTexture;
    std::shared_ptr<Texture2D> m_MetalicTexture;
    
    glm::vec3 m_BaseColor = glm::vec3(1.0f, 1.0f, 1.0f);

    float m_Roughness = 1.0f;
    float m_Metalic = 1.0f;
    float m_Emission = 0.0f;
};
