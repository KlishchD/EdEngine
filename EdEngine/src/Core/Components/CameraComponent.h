#pragma once

#include "Component.h"
#include "Core/Math/Camera.h"

class CameraComponent : public Component
{
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const uint32_t version)
	{
		ar & boost::serialization::base_object<Component>(*this);
		ar & m_Camera;
	}
public:

	void SetCamera(const Camera& camera);	
	Camera& GetCamera();

protected:
	Camera m_Camera;
};
