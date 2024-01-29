#pragma once

#include "Component.h"

class PointLightComponent: public Component
{
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const uint32_t version)
    {
        ar & boost::serialization::base_object<Component>(*this);
        ar & m_Color;
        ar & m_Intensity;
        ar & m_Radius;
        ar & m_bIsCastingShadows;

        m_Color = glm::clamp(m_Color, glm::vec3(0.0f), glm::vec3(1.0f));
    }

public:
    PointLightComponent();
    PointLightComponent(const PointLightComponent& PointLightComponent);

    glm::vec3 GetPosition() const;

    void SetColor(glm::vec3 color);
    glm::vec3 GetColor() const { return m_Color; }

    void SetIntensity(float intensity);
    float GetIntensity();
    
    bool IsShadowCasting() const;
    void SetShadowCasting(bool isShadowCasting);

    void SetRadius(float radius);
    float GetRadius() const;
    
    virtual ComponentType GetType() const override;
    
    glm::mat4 GetShadowMapPassCameraTransformation(int32_t index) const;
private:
    glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f};
    float m_Intensity = 1.0f;
    bool m_bIsCastingShadows = true;
    float m_Radius = 1.0f;
};

BOOST_CLASS_EXPORT_KEY(PointLightComponent)
BOOST_CLASS_VERSION(PointLightComponent, 1)