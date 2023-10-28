#include "Camera.h"

#include <glm/gtc/quaternion.hpp>
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(float fov, float aspect, float near, float far): m_Projection(glm::perspective(glm::radians(fov), aspect, near, far))
{
}

Camera::Camera(float fov, float aspect, float near, float far, glm::vec3 rotation, glm::vec3 position): m_Projection(glm::perspective(glm::radians(fov), aspect, near, far)), m_Position(position)
{
    SetRotation(rotation);
}

void Camera::SetProjection(float fov, float aspect, float near, float far)
{
    m_Projection = glm::perspective(glm::radians(fov), aspect, near, far);
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

glm::mat4 Camera::GetMatrix() const
{
    return m_Projection * glm::lookAt(m_Position, m_Position + m_Orientation, m_Up); // TODO: May be add some checks here ;)
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
