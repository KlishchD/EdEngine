#include "PointLightComponent.h"
#include <glm/gtx/quaternion.hpp>

PointLightComponent::PointLightComponent()
{
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

uint32_t PointLightComponent::GetShadowFilterSize() const
{
    return m_ShadowFilterSize;
}

void PointLightComponent::SetShadowFilterSize(uint32_t size)
{
    m_ShadowFilterSize = size;
}

void PointLightComponent::Serialize(Archive& archive)
{
    Super::Serialize(archive);

    archive & m_Radius;
    archive & m_ShadowFilterSize;
}

void PointLightComponent::SetRadius(float radius)
{
    m_Radius = radius;
}

float PointLightComponent::GetRadius() const
{
    return m_Radius;
}
