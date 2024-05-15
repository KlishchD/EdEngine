#include "DirectionalLightComponent.h"

ComponentType DirectionalLightComponent::GetType() const
{
	return ComponentType::DirectionalLight;
}

glm::vec3 DirectionalLightComponent::GetDirection() const
{
	const glm::quat rotation = GetRelativeTransform().GetRotation();
	const glm::vec3 direction(0.0f, -1.0f, 0.0f);
	return glm::normalize(rotation * direction);
}

uint32_t DirectionalLightComponent::GetShadowCascadesCount() const
{
	return m_CascadesCount;
}

void DirectionalLightComponent::SetShadowCascadesCount(uint32_t count)
{
	m_CascadesCount = count;
}

float DirectionalLightComponent::GetShadowMapZMultiplier() const
{
	return m_ShadowMapZMultiplier;
}

void DirectionalLightComponent::SetShadowMapZMultiplier(float multiplier)
{
	m_ShadowMapZMultiplier = multiplier;
}

uint32_t DirectionalLightComponent::GetShadowFilterSize() const
{
	return m_ShadowFilterSize;
}

void DirectionalLightComponent::SetShadowFilterSize(uint32_t size)
{
	m_ShadowFilterSize = size;
}

float DirectionalLightComponent::GetShadowFilterRadius() const
{
	return m_ShadowFilterRadius;
}

void DirectionalLightComponent::SetShadowFilterRadius(float radius)
{
	m_ShadowFilterRadius = radius;
}

void DirectionalLightComponent::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & m_CascadesCount;
	archive & m_ShadowMapZMultiplier;
	archive & m_ShadowFilterSize;
	archive & m_ShadowFilterRadius;
}
