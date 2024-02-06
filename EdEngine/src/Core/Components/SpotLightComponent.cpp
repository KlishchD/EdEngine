#include "SpotLightComponent.h"

BOOST_CLASS_EXPORT_IMPLEMENT(SpotLightComponent)


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

ComponentType SpotLightComponent::GetType() const
{
	return ComponentType::SpotLight;
}
