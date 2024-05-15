#pragma once

#include "LightComponent.h"

ED_CLASS(SpotLightComponent) : public LightComponent
{
	ED_CLASS_BODY(SpotLightComponent, LightComponent)
public:
	void SetInnerAngle(float angle);
	float GetInnerAngle() const;

	void SetOuterAngle(float angle);
	float GetOuterAngle() const;

	void SetMaxDistance(float distance);
	float GetMaxDistance() const;

	void SetShadowFilterSize(uint32_t size);
	uint32_t GetShadowFilterSize() const;

	void SetShadowFilterRadius(float radius);
	float GetShadowFilterRadius() const;

	virtual ComponentType GetType() const override;

	virtual void Serialize(Archive& archive) override;
private:
	float m_InnerAngle = 1.0f;
	float m_OuterAngle = 1.0f;
	float m_MaxDistance = 1.0f;

	uint32_t m_ShadowFilterSize = 4;
	float m_ShadowFilterRadius = 3.0f;
};