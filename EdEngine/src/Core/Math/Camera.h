#pragma once
#include <glm/fwd.hpp>

#include "Transform.h"

class Camera
{
public:
    Camera();
    Camera(float fovDegrees, float aspect, float near, float far);
    Camera(float fovDegrees, float aspect, float near, float far, glm::vec3 rotation, glm::vec3 position);

    void SetProjection(float fov, float aspect, float near, float far);
    void SetProjection(const glm::mat4& projection);

    glm::vec3 GetPosition() const;
    void SetPosition(const glm::vec3 position);

    glm::vec3 GetOrientation() const;
    void SetOrientation(glm::vec3 orientation);

    void AddRotation(glm::vec3 rotation);
    void SetRotation(glm::vec3 rotation);

    glm::vec3 GetRotation() const;
    
    void AddPositionOffset(glm::vec3 offset);

    glm::mat4 GetView() const;
    glm::mat4 GetProjection() const;
    glm::mat4 GetProjectionView() const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    glm::mat4 GetProjection(float near, float far) const;
    glm::mat4 GetProjectionView(float near, float far) const;

    void SetFOVRadians(float fov);
    void SetFOVDegress(float fov);
    float GetFOVRadians() const;
    float GetFOVDegrees() const;

    void SetAspect(float aspect);
    float GetAspect() const;

    void SetNear(float near);
    float GetNear() const;

    void SetFar(float far);
    float GetFar() const;

    void Serialize(Archive& ar);
private:
    glm::mat4 m_Projection;
    
    glm::vec3 m_Position = glm::vec3(0.0f);

    glm::vec3 m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    
    glm::vec3 m_Orientation = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_Fov = 0;
    float m_Aspect = 0;
    float m_Near = 0;
    float m_Far = 0;

    void CalculateProjectionMatrix();
};
