#pragma once

#include "LightComponent.h"

ED_CLASS(DirectionalLightComponent) : public LightComponent
{
	ED_CLASS_BODY(DirectionalLightComponent, LightComponent)
public:
	virtual ComponentType GetType() const override;

	glm::vec3 GetDirection() const;

	uint32_t GetShadowCascadesCount() const;
	void SetShadowCascadesCount(uint32_t count);

	float GetShadowMapZMultiplier() const;
	void SetShadowMapZMultiplier(float multiplier);

	uint32_t GetShadowFilterSize() const;
	void SetShadowFilterSize(uint32_t size);

	float GetShadowFilterRadius() const;
	void SetShadowFilterRadius(float radius);

	virtual void Serialize(Archive& archive) override;
private:
	uint32_t m_CascadesCount = 4;
	float m_ShadowMapZMultiplier = 10.0f;
	
	uint32_t m_ShadowFilterSize = 3;
	float m_ShadowFilterRadius = 3.0f;
};
