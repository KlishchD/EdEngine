#pragma once

#include "Actor.h"
#include "Core/Components/CameraComponent.h"

ED_CLASS(PlayerActor) : public Actor
{
	ED_CLASS_BODY(PlayerActor, Actor)
public:
	PlayerActor(const std::string& name = "empty");

	virtual void Intialize() override;

	std::shared_ptr<CameraComponent> GetCameraComponent() const;

protected:
	std::shared_ptr<CameraComponent> m_CameraComponent;
};