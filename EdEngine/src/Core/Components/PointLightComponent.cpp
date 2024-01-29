#include "PointLightComponent.h"
#include <glm/gtx/quaternion.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(PointLightComponent)

PointLightComponent::PointLightComponent(): Component("Light") { }

PointLightComponent::PointLightComponent(const PointLightComponent& PointLightComponent) : Component(PointLightComponent.GetName())
{
    m_Transform = PointLightComponent.m_Transform;
    m_Color = PointLightComponent.m_Color;
    m_Intensity = PointLightComponent.m_Intensity;
    m_Intensity = PointLightComponent.m_Radius;
    m_Intensity = PointLightComponent.m_bIsCastingShadows;
}

glm::vec3 PointLightComponent::GetPosition() const
{
    return m_Transform.GetTranslation();
}

void PointLightComponent::SetColor(glm::vec3 color)
{
    m_Color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
}

ComponentType PointLightComponent::GetType() const
{
    return ComponentType::PointLight;
}

glm::mat4 PointLightComponent::GetShadowMapPassCameraTransformation(int32_t index) const
{
    switch (index) // TODO: experiment here ;)
    {
    case 0: return glm::lookAt(m_Transform.GetTranslation(), m_Transform.GetTranslation() + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    case 1: return glm::lookAt(m_Transform.GetTranslation(), m_Transform.GetTranslation() + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    case 2: return glm::lookAt(m_Transform.GetTranslation(), m_Transform.GetTranslation() + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
    case 3: return glm::lookAt(m_Transform.GetTranslation(), m_Transform.GetTranslation() + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
    case 4: return glm::lookAt(m_Transform.GetTranslation(), m_Transform.GetTranslation() + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    case 5: return glm::lookAt(m_Transform.GetTranslation(), m_Transform.GetTranslation() + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    default: return glm::mat4(1);
    }
}

void PointLightComponent::SetIntensity(float intensity)
{
    m_Intensity = intensity;
}

float PointLightComponent::GetIntensity()
{
    return m_Intensity;
}

bool PointLightComponent::IsShadowCasting() const
{
    return m_bIsCastingShadows;
}

void PointLightComponent::SetShadowCasting(bool isShadowCasting)
{
    m_bIsCastingShadows = isShadowCasting;
}

void PointLightComponent::SetRadius(float radius)
{
    m_Radius = radius;
}

float PointLightComponent::GetRadius() const
{
    return m_Radius;
}
