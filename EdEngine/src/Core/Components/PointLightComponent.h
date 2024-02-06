#pragma once

#include "LightComponent.h"

class PointLightComponent: public LightComponent
{
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const uint32_t version)
    {
        ar & boost::serialization::base_object<LightComponent>(*this);
        ar & m_Radius;
    }

public:
    PointLightComponent();

    void SetRadius(float radius);
    float GetRadius() const;
    
    virtual ComponentType GetType() const override;
    
    glm::mat4 GetShadowMapPassCameraTransformation(int32_t index) const;
private:
    float m_Radius = 1.0f;
};

BOOST_CLASS_EXPORT_KEY(PointLightComponent)
BOOST_CLASS_VERSION(PointLightComponent, 1)