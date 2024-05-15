#pragma once

#include "Component.h"
#include "Core/Math/Camera.h"

ED_CLASS(CameraComponent) : public Component
{
	ED_CLASS_BODY(CameraComponent, Component)
public:
	CameraComponent(const std::string& name = "None");

	void SetCamera(const Camera& camera);	
	Camera& GetCamera();

	virtual void Serialize(Archive& archive) override;
protected:
	Camera m_Camera;
};