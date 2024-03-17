#include "CameraComponent.h"

void CameraComponent::SetCamera(const Camera& camera)
{
	m_Camera = camera;
}

Camera& CameraComponent::GetCamera()
{
	return m_Camera;
}
