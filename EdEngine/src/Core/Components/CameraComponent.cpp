#include "CameraComponent.h"

CameraComponent::CameraComponent(const std::string& name) : Super(name)
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
	Super::Serialize(archive);

	archive & m_Camera;
}
