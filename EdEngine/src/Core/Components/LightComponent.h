#pragma once

#include "Component.h"

ED_CLASS(LightComponent) : public Component
{
	ED_CLASS_BODY(LightComponent, Component)
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

	virtual void Serialize(Archive& archive) override;
private:
	glm::vec3 m_Color = glm::vec3(1.0f);
	float m_Intensity = glm::radians(1.0f);
	bool m_bIsCastingShadow = true;
	bool m_bShowWireframe = false;
};