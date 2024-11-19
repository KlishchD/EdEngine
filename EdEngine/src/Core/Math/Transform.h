#pragma once

#include "Helpers/SerializationHelper.h"

class Transform
{
public:
    Transform() = default;
    Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale);
    Transform(glm::vec3 translation, glm::quat rotation);
    
    void SetTranslation(const glm::vec3& translation) { m_Translation = translation; }
    void SetTranslation(float x, float y, float z) { m_Translation = glm::vec3(x, y, z); }
    
    void SetRotation(const glm::quat& rotation) { m_Rotation = rotation; }
    void SetRotation(float pitch, float yaw, float roll) { m_Rotation = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));}

    void SetScale(const glm::vec3& scale) { m_Scale = scale; }
    void SetScale(float x, float y, float z) { m_Scale = glm::vec3(x, y, z); }

    inline glm::mat4 GetMatrix() const;
    glm::mat4 GetMatrixWithOutScale() const;

    glm::mat4 GetInversedTransposedMatrix() const;

    Transform operator+(const Transform& transform) const;
    Transform& operator=(const Transform& transform);

    glm::vec3 GetTranslation() const;
    glm::quat GetRotation() const;
    glm::vec3 GetScale() const;

    void AddTranslation(glm::vec3 offset);
    void AddRotation(glm::vec3 rotation);
    void AddScale(glm::vec3 scale);

    glm::vec3 GetEulerRotation() const;

    void Serialize(Archive& ar);
private:
    glm::vec3 m_Translation = glm::vec3(0.0f);
    glm::quat m_Rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec3 m_Scale = glm::vec3(1.0f);
};
