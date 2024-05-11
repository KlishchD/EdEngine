#include "CameraComponent.h"

CameraComponent::CameraComponent(const std::string& name) : Component(name)
{

}

void CameraComponent::SetCamera(const Camera& camera)
{
	m_Camera = camera;
}

Camera& CameraComponent::GetCamera()
{
	return m_Camera;
}

void CameraComponent::Serialize(Archive& archive)
{
	Component::Serialize(archive);

	archive & m_Camera;
}
