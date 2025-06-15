#pragma once

class Transform
{
public:
    static Transform Unit;
public:
    Transform() = default;
    Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale);
    Transform(glm::vec3 translation, glm::quat rotation);
    
    void SetTranslation(const glm::vec3& translation) { m_Translation = translation; }
    void SetTranslation(f32 x, f32 y, f32 z) { m_Translation = glm::vec3(x, y, z); }
    
    void SetRotation(const glm::quat& rotation) { m_Rotation = rotation; }
    void SetRotation(f32 pitch, f32 yaw, f32 roll) { m_Rotation = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));}

    void SetScale(const glm::vec3& scale) { m_Scale = scale; }
    void SetScale(f32 x, f32 y, f32 z) { m_Scale = glm::vec3(x, y, z); }

    glm::mat4 GetMatrix() const;
    glm::mat4 GetMatrixWithOutScale() const;
    glm::mat4 GetMatrixWithOutTranslationAndScale() const;

    glm::mat4 GetInversedTransposedMatrix() const;

    Transform operator+(const Transform& transform) const;
    Transform& operator=(const Transform& transform);

    glm::vec3 GetTranslation() const;
    glm::quat GetRotation() const;
    glm::vec3 GetScale() const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    void AddTranslation(glm::vec3 offset);
    void AddRotation(glm::vec3 rotation);
    void AddScale(glm::vec3 scale);

    glm::vec3 GetEulerRotation() const;

    void Serialize(Archive<>& ar);
private:
    glm::vec3 m_Translation = glm::vec3(0.0f);
    glm::quat m_Rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec3 m_Scale = glm::vec3(1.0f);
};