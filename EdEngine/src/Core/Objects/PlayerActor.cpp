#include "PlayerActor.h"

PlayerActor::PlayerActor(const std::string& name) : Super(name)
{
}

void PlayerActor::Intialize()
{
	for (const std::shared_ptr<Component>& component : m_Components)
	{
		if (std::shared_ptr<CameraComponent> cameraComponent = std::dynamic_pointer_cast<CameraComponent>(component))
		{
			m_CameraComponent = cameraComponent;
			break;
		}
	}

	if (!m_CameraComponent)
	{
		m_CameraComponent = std::make_shared<CameraComponent>("Camera");
		RegisterComponent(m_CameraComponent);
	}
}

std::shared_ptr<CameraComponent> PlayerActor::GetCameraComponent() const
{
	return m_CameraComponent;
}
