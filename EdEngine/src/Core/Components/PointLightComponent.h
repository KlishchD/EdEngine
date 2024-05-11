#pragma once

#include "LightComponent.h"

class PointLightComponent: public LightComponent
{
public:
    PointLightComponent();

    void SetRadius(float radius);
    float GetRadius() const;
    
    virtual ComponentType GetType() const override;
    
    glm::mat4 GetShadowMapPassCameraTransformation(int32_t index) const;

    uint32_t GetShadowFilterSize() const;
    void SetShadowFilterSize(uint32_t size);

    virtual void Serialize(Archive& archive) override;
private:
    float m_Radius = 1.0f;
    uint32_t m_ShadowFilterSize = 3;
};
