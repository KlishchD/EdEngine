#pragma once

#include "LightComponent.h"

class SpotLightComponent : public LightComponent
{
	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const uint32_t version)
	{
		ar & boost::serialization::base_object<LightComponent>(*this);
		ar & m_InnerAngle;
		ar & m_OuterAngle;
		ar & m_MaxDistance;

		if (version > 1)
		{
			ar & m_ShadowFilterSize;
			ar & m_ShadowFilterRadius;
		}
	}
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
private:
	float m_InnerAngle = 1.0f;
	float m_OuterAngle = 1.0f;
	float m_MaxDistance = 1.0f;

	uint32_t m_ShadowFilterSize = 4;
	float m_ShadowFilterRadius = 3.0f;
};

BOOST_CLASS_EXPORT_KEY(SpotLightComponent)
BOOST_CLASS_VERSION(SpotLightComponent, 2)