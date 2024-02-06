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
	}
public:
	void SetInnerAngle(float angle);
	float GetInnerAngle() const;

	void SetOuterAngle(float angle);
	float GetOuterAngle() const;

	void SetMaxDistance(float distance);
	float GetMaxDistance() const;

	virtual ComponentType GetType() const override;
private:
	float m_InnerAngle = 1.0f;
	float m_OuterAngle = 1.0f;
	float m_MaxDistance = 1.0f;
};

BOOST_CLASS_EXPORT_KEY(SpotLightComponent)
BOOST_CLASS_VERSION(SpotLightComponent, 1)