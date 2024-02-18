#pragma once

#include "LightComponent.h"

class DirectionalLightComponent : public LightComponent
{
	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const uint32_t version)
	{
		ar & boost::serialization::base_object<LightComponent>(*this);
		
		if (version > 1)
		{
			ar & m_CascadesCount;
			ar & m_ShadowMapZMultiplier;
		}

		if (version > 2)
		{
			ar & m_ShadowFilterSize;
			ar & m_ShadowFilterRadius;
		}
	}
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
private:
	uint32_t m_CascadesCount = 4;
	float m_ShadowMapZMultiplier = 10.0f;
	
	uint32_t m_ShadowFilterSize = 3;
	float m_ShadowFilterRadius = 3.0f;
};

BOOST_CLASS_EXPORT_KEY(DirectionalLightComponent)
BOOST_CLASS_VERSION(DirectionalLightComponent, 3)