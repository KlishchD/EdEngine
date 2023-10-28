#pragma once

#include "Asset.h"
#include "Shader.h"

struct MaterialDescriptor: public AssetDescriptor
{
    std::string ShaderPath;
    
    glm::vec4 BaseColor = glm::vec4(1.0f);
    float Roughness = 0.5f;
    float Metalic = 0.5f;
    
    int32_t BaseColorTextureID = -1;
    int32_t NormalTextureID = -1;
    int32_t RoughnessTextureID = -1;
    int32_t MetalicTextureID = -1;
};

namespace boost
{
    namespace serialization
    {
        template <class Archive>
        void serialize(Archive& ar, MaterialDescriptor& descriptor, uint32_t version)
        {
            ar & boost::serialization::base_object<AssetDescriptor>(descriptor);

            ar & descriptor.BaseColor.x;
            ar & descriptor.BaseColor.y;
            ar & descriptor.BaseColor.z;
            
            ar & descriptor.Roughness;
            ar & descriptor.Metalic;
            
            ar & descriptor.ShaderPath;
            ar & descriptor.BaseColorTextureID;
            ar & descriptor.NormalTextureID;
            ar & descriptor.RoughnessTextureID;
            ar & descriptor.MetalicTextureID;
        }
    }
}

class Texture2D;

class Material : public Asset
{
public:
    Material(const Material& material);
    Material();

    void SetShaderData(const std::shared_ptr<Shader>& shared);
    
    void SetBaseColor(glm::vec4 color) { m_BaseColor = color; }
    void SetRoughness(float roughness) { m_Roughness = roughness; }
    void SetMetalic(float metalic) { m_Metalic = metalic; }

    void SetBaseColorTexture(const std::shared_ptr<Texture2D>& texture);
    void SetNormalTexture(const std::shared_ptr<Texture2D>& texture);
    void SetRoughnessTexture(const std::shared_ptr<Texture2D>& texture);
    void SetMetalicTexture(const std::shared_ptr<Texture2D>& texture);
private:

    std::shared_ptr<Texture2D> m_BaseColorTexture;
    std::shared_ptr<Texture2D> m_NormalTexture;
    
    std::shared_ptr<Texture2D> m_RoughnessTexture;
    std::shared_ptr<Texture2D> m_MetalicTexture;
    
    glm::vec4 m_BaseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    float m_Roughness = 0.5f;
    float m_Metalic = 0.5f;
};

BOOST_CLASS_EXPORT_KEY(MaterialDescriptor)
BOOST_CLASS_VERSION(MaterialDescriptor, 1)