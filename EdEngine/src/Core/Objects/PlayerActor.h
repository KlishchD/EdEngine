#pragma once

#include "Actor.h"
#include "Core/Components/CameraComponent.h"

class PlayerActor : public Actor
{
public:
	PlayerActor(const std::string& name = "empty");

	std::shared_ptr<CameraComponent> GetCameraComponent() const;

protected:
	std::shared_ptr<CameraComponent> m_CameraComponent;
};
