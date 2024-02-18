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

        if (version > 1)
        {
            ar & m_ShadowFilterSize;
        }
    }

public:
    PointLightComponent();

    void SetRadius(float radius);
    float GetRadius() const;
    
    virtual ComponentType GetType() const override;
    
    glm::mat4 GetShadowMapPassCameraTransformation(int32_t index) const;

    uint32_t GetShadowFilterSize() const;
    void SetShadowFilterSize(uint32_t size);
private:
    float m_Radius = 1.0f;
    uint32_t m_ShadowFilterSize = 3;
};

BOOST_CLASS_EXPORT_KEY(PointLightComponent)
BOOST_CLASS_VERSION(PointLightComponent, 2)