#include "LightComponent.h"

BOOST_CLASS_EXPORT_IMPLEMENT(LightComponent)

LightComponent::LightComponent(): Component("Light")
{
}

void LightComponent::SetColor(glm::vec3 color)
{
	m_Color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
}

glm::vec3 LightComponent::GetColor() const
{
	return m_Color;
}

void LightComponent::SetIntensity(float intensity)
{
	m_Intensity = intensity;
}

float LightComponent::GetIntensity() const
{
	return m_Intensity;
}

bool LightComponent::IsShadowCasting() const
{
	return m_bIsCastingShadow;
}

void LightComponent::SetShowWireframe(bool enabled)
{
	m_bShowWireframe = enabled;
}

bool LightComponent::ShouldShowWireframe() const
{
	return m_bShowWireframe;
}

glm::vec3 LightComponent::GetPosition() const
{
	return GetWorldTransform().GetTranslation();
}

void LightComponent::SetShadowCasting(bool enabled)
{
	m_bIsCastingShadow = enabled;
}
