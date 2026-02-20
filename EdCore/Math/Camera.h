#pragma once

#pragma message("Caching!!!!")
class Camera
{
public:
    Camera();
    Camera(f32 fovDegrees, f32 aspect, f32 near, f32 far);
    Camera(f32 fovDegrees, f32 aspect, f32 near, f32 far, glm::vec3 rotation, glm::vec3 position);

    void SetProjection(f32 fov, f32 aspect, f32 near, f32 far);
    void SetProjection(const glm::mat4& projection);

    glm::vec3 GetPosition() const;
    void SetPosition(const glm::vec3 position);

    glm::vec3 GetOrientation() const;
    void SetOrientation(glm::vec3 orientation);

    void AddRotation(glm::vec3 rotation);
    void SetRotation(glm::vec3 rotation);
    void SetRotation(glm::quat rotation);

    glm::vec3 GetRotation() const;
    
    void AddPositionOffset(glm::vec3 offset);

    glm::mat4 GetView() const;
    glm::mat4 GetProjection() const;
    glm::mat4 GetProjectionView() const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    glm::mat4 GetProjection(f32 near, f32 far) const;
    glm::mat4 GetProjectionView(f32 near, f32 far) const;

    void SetFOVRadians(f32 fov);
    void SetFOVDegress(f32 fov);
    f32 GetFOVRadians() const;
    f32 GetFOVDegrees() const;

    void SetAspect(f32 aspect);
    f32 GetAspect() const;

    void SetNear(f32 near);
    f32 GetNear() const;

    void SetFar(f32 far);
    f32 GetFar() const;

    void Serialize(Archive<>& ar);
private:
    glm::mat4 m_Projection;
    
    glm::vec3 m_Position = glm::vec3(0.0f);

    glm::vec3 m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    
    glm::vec3 m_Orientation = glm::forward;
    glm::vec3 m_Up = glm::up;

    f32 m_Fov = 0;
    f32 m_Aspect = 0;
    f32 m_Near = 0;
    f32 m_Far = 0;

    void CalculateProjectionMatrix();
};
