#include "Camera.h"

#include <glm/gtc/quaternion.hpp>
#include "glm/gtx/rotate_vector.hpp"

#undef far
#undef near

Camera::Camera() : m_Projection(glm::mat4(1))
{

}

Camera::Camera(float fovDegrees, float aspect, float near, float far): m_Fov(glm::radians(fovDegrees)), m_Aspect(aspect), m_Near(near), m_Far(far)
{
    CalculateProjectionMatrix();
}

Camera::Camera(float fovDegrees, float aspect, float near, float far, glm::vec3 rotation, glm::vec3 position): m_Fov(glm::radians(fovDegrees)), m_Aspect(aspect), m_Near(near), m_Far(far), m_Position(position)
{
    SetRotation(rotation);
    CalculateProjectionMatrix();
}

void Camera::SetProjection(float fov, float aspect, float near, float far)
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
    
    if (m_Rotation.x > 89.0f)
    {
        m_Rotation.x = -89.0f;
    }
    else if (m_Rotation.x < -89.0f)
    {
        m_Rotation.x = 89.0f;
    }

    float pitch = glm::radians(m_Rotation.x);
    float yaw = glm::radians(m_Rotation.y);
    
    m_Orientation.x = glm::cos(pitch) * glm::cos(yaw);
    m_Orientation.y = glm::sin(pitch);
    m_Orientation.z = glm::cos(pitch) * glm::sin(yaw);
    m_Orientation = glm::normalize(m_Orientation);
}

void Camera::SetRotation(glm::vec3 rotation)
{
    m_Rotation = rotation;
    
    if (m_Rotation.x > 89.0f)
    {
        m_Rotation.x = -89.0f;
    }
    else if (m_Rotation.x < -89.0f)
    {
        m_Rotation.x = 89.0f;
    }

    float pitch = glm::radians(m_Rotation.x);
    float yaw = glm::radians(m_Rotation.y);
    
    m_Orientation.x = glm::cos(pitch) * glm::cos(yaw);
    m_Orientation.y = glm::sin(pitch);
    m_Orientation.z = glm::cos(pitch) * glm::sin(yaw);
    m_Orientation = glm::normalize(m_Orientation);
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
    return glm::cross(m_Orientation, m_Up);
}

glm::vec3 Camera::GetUp() const
{
    return m_Up;
}

glm::mat4 Camera::GetProjection(float near, float far) const
{
    return glm::perspective(m_Fov, m_Aspect, near, far);
}

glm::mat4 Camera::GetProjectionView(float near, float far) const
{
    glm::mat4 projection = GetProjection(near, far);
    glm::mat4 view = GetView();
    return projection * view;
}

void Camera::SetFOVRadians(float fov)
{
    m_Fov = fov;
    CalculateProjectionMatrix();
}

void Camera::SetFOVDegress(float fov)
{
    m_Fov = glm::radians(fov);
    CalculateProjectionMatrix();
}

float Camera::GetFOVRadians() const
{
    return m_Fov;
}

float Camera::GetFOVDegrees() const
{
    return glm::degrees(m_Fov);
}

void Camera::SetAspect(float aspect)
{
    m_Aspect = aspect;
    CalculateProjectionMatrix();
}

float Camera::GetAspect() const
{
    return m_Aspect;
}

void Camera::SetNear(float near)
{
    m_Near = near;
    CalculateProjectionMatrix();
}

float Camera::GetNear() const
{
    return m_Near;
}

void Camera::SetFar(float far)
{
    m_Far = far;
    CalculateProjectionMatrix();
}

float Camera::GetFar() const
{
    return m_Far;
}

void Camera::Serialize(Archive& ar)
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
    m_Projection = glm::perspective(m_Fov, m_Aspect, m_Near, m_Far);
}
