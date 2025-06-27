#include "EdCore.h"
#include "Camera.h"
#include "Archive.h"

Camera::Camera() : m_Projection(glm::mat4(1))
{

}

Camera::Camera(f32 fovDegrees, f32 aspect, f32 near, f32 far): m_Fov(glm::radians(fovDegrees)), m_Aspect(aspect), m_Near(near), m_Far(far)
{
    CalculateProjectionMatrix();
}

Camera::Camera(f32 fovDegrees, f32 aspect, f32 near, f32 far, glm::vec3 rotation, glm::vec3 position): m_Fov(glm::radians(fovDegrees)), m_Aspect(aspect), m_Near(near), m_Far(far), m_Position(position)
{
    SetRotation(rotation);
    CalculateProjectionMatrix();
}

void Camera::SetProjection(f32 fov, f32 aspect, f32 near, f32 far)
{
    m_Fov = glm::radians(fov);
    m_Aspect = aspect;
    m_Near = near;
    m_Far = far;
    CalculateProjectionMatrix();
}

void Camera::SetProjection(const glm::mat4& projection)
{
    m_Projection = projection;
}

glm::vec3 Camera::GetPosition() const
{
    return m_Position;
}

void Camera::SetPosition(const glm::vec3 position)
{
    m_Position = position;
}

glm::vec3 Camera::GetOrientation() const
{
    return m_Orientation;
}

glm::vec3 Camera::GetRotation() const
{
    return m_Rotation;
}

void Camera::SetOrientation(glm::vec3 orientation)
{
    m_Orientation = orientation;
}

void Camera::AddRotation(glm::vec3 rotation)
{
    m_Rotation.x += rotation.x;
    m_Rotation.y += rotation.y;

    m_Orientation = glm::quat(glm::radians(m_Rotation)) * glm::forward;
}

void Camera::SetRotation(glm::vec3 rotation)
{
    m_Rotation = rotation;

    m_Orientation = glm::quat(glm::radians(m_Rotation)) * glm::forward;
}

void Camera::SetRotation(glm::quat rotation)
{
    SetRotation(glm::eulerAngles(rotation));
}

void Camera::AddPositionOffset(glm::vec3 offset)
{
    m_Position += offset;
}

glm::mat4 Camera::GetView() const
{
    return glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}

glm::mat4 Camera::GetProjection() const
{
    return m_Projection;
}

glm::mat4 Camera::GetProjectionView() const
{
    return m_Projection * GetView(); // TODO: May be add some checks here ;)
}

glm::vec3 Camera::GetForward() const
{
    return m_Orientation;
}

glm::vec3 Camera::GetRight() const
{
    return glm::cross(m_Up, m_Orientation);
}

glm::vec3 Camera::GetUp() const
{
    return m_Up;
}

glm::mat4 Camera::GetProjection(f32 near, f32 far) const
{
    return glm::perspective(m_Fov, m_Aspect, far, near);
}

glm::mat4 Camera::GetProjectionView(f32 near, f32 far) const
{
    glm::mat4 projection = GetProjection(near, far);
    glm::mat4 view = GetView();
    return projection * view;
}

void Camera::SetFOVRadians(f32 fov)
{
    m_Fov = fov;
    CalculateProjectionMatrix();
}

void Camera::SetFOVDegress(f32 fov)
{
    m_Fov = glm::radians(fov);
    CalculateProjectionMatrix();
}

f32 Camera::GetFOVRadians() const
{
    return m_Fov;
}

f32 Camera::GetFOVDegrees() const
{
    return glm::degrees(m_Fov);
}

void Camera::SetAspect(f32 aspect)
{
    m_Aspect = aspect;
    CalculateProjectionMatrix();
}

f32 Camera::GetAspect() const
{
    return m_Aspect;
}

void Camera::SetNear(f32 near)
{
    m_Near = near;
    CalculateProjectionMatrix();
}

f32 Camera::GetNear() const
{
    return m_Near;
}

void Camera::SetFar(f32 far)
{
    m_Far = far;
    CalculateProjectionMatrix();
}

f32 Camera::GetFar() const
{
    return m_Far;
}

void Camera::Serialize(Archive<>& ar)
{
    ar & m_Position;
    ar & m_Rotation;
    ar & m_Orientation;
    ar & m_Up;

    ar & m_Fov;
    ar & m_Aspect;
    ar & m_Near;
    ar & m_Far;
}

void Camera::CalculateProjectionMatrix()
{
    m_Projection = glm::perspective(m_Fov, m_Aspect, m_Far, m_Near);
}
