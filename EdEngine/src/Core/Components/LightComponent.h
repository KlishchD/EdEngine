#pragma once

#include "Component.h"

class LightComponent : public Component
{
	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const uint32_t version)
	{
		ar & boost::serialization::base_object<Component>(*this);
		ar & m_Color;
		ar & m_Intensity;
		ar & m_bIsCastingShadow;

		m_Color = glm::clamp(m_Color, glm::vec3(0.0f), glm::vec3(1.0f));
	}
public:
	LightComponent();

	void SetColor(glm::vec3 color);
	glm::vec3 GetColor() const;

	void SetIntensity(float intensity);
	float GetIntensity() const;

	void SetShadowCasting(bool enabled);
	bool IsShadowCasting() const;

	void SetShowWireframe(bool enabled);
	bool ShouldShowWireframe() const;

	glm::vec3 GetPosition() const;
private:
	glm::vec3 m_Color = glm::vec3(1.0f);
	float m_Intensity = glm::radians(1.0f);
	bool m_bIsCastingShadow = true;
	bool m_bShowWireframe = false;
};

BOOST_CLASS_EXPORT_KEY(LightComponent)
BOOST_CLASS_VERSION(LightComponent, 1)