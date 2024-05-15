#include "SpotLightComponent.h"

void SpotLightComponent::SetInnerAngle(float angle)
{
	m_InnerAngle = angle;
	m_OuterAngle = glm::max(m_OuterAngle, angle);
}

float SpotLightComponent::GetInnerAngle() const
{
	return m_InnerAngle;
}

void SpotLightComponent::SetOuterAngle(float angle)
{
	m_OuterAngle = angle;
	m_InnerAngle = glm::min(m_InnerAngle, angle);
}

float SpotLightComponent::GetOuterAngle() const
{
	return m_OuterAngle;
}

void SpotLightComponent::SetMaxDistance(float distance)
{
	m_MaxDistance = distance;
}

float SpotLightComponent::GetMaxDistance() const
{
	return m_MaxDistance;
}

void SpotLightComponent::SetShadowFilterSize(uint32_t size)
{
	m_ShadowFilterSize = size;
}

uint32_t SpotLightComponent::GetShadowFilterSize() const
{
	return m_ShadowFilterSize;
}

void SpotLightComponent::SetShadowFilterRadius(float radius)
{
	m_ShadowFilterRadius = radius;
}

float SpotLightComponent::GetShadowFilterRadius() const
{
	return m_ShadowFilterRadius;
}

ComponentType SpotLightComponent::GetType() const
{
	return ComponentType::SpotLight;
}

void SpotLightComponent::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & m_InnerAngle;
	archive & m_OuterAngle;
	archive & m_MaxDistance;
	archive & m_ShadowFilterSize;
	archive & m_ShadowFilterRadius;
}
