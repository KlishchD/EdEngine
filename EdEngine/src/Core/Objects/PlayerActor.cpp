#include "PlayerActor.h"

PlayerActor::PlayerActor(const std::string& name) : Actor(name)
{
	m_CameraComponent = std::make_shared<CameraComponent>();
	//RegisterComponent(m_CameraComponent);
}

std::shared_ptr<CameraComponent> PlayerActor::GetCameraComponent() const
{
	return m_CameraComponent;
}
