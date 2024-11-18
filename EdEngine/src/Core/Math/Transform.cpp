#include "Transform.h"

#include <glm/gtc/quaternion.hpp>

Transform::Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale): m_Translation(translation), m_Rotation(rotation), m_Scale(scale)
{
    
}

Transform::Transform(glm::vec3 translation, glm::quat rotation): m_Translation(translation), m_Rotation(rotation)
{
    
}

glm::mat4 Transform::GetMatrix() const
{
    glm::mat4 mat = glm::mat4(1.0f);
    mat = glm::translate(mat, m_Translation);
    mat = glm::rotate(mat, glm::angle(m_Rotation), glm::axis(m_Rotation));
    mat = glm::scale(mat, m_Scale);
    return mat;
}

glm::mat4 Transform::GetMatrixWithOutScale() const
{
    glm::mat4 mat = glm::translate(glm::mat4(1.0f), m_Translation);
    mat = glm::rotate(mat, glm::angle(m_Rotation), glm::axis(m_Rotation));
    return mat;
}

glm::mat4 Transform::GetInversedTransposedMatrix() const
{
    return glm::inverse(glm::transpose(GetMatrix()));
}

glm::vec3 Transform::GetTranslation() const
{
    return m_Translation;
}

glm::quat Transform::GetRotation() const
{
    return m_Rotation;
}

glm::vec3 Transform::GetScale() const
{
    return m_Scale;
}

void Transform::AddTranslation(glm::vec3 offset)
{
    m_Translation += offset;
}

void Transform::AddRotation(glm::vec3 rotation)
{
    rotation.x = glm::radians(rotation.x);
    rotation.y = glm::radians(rotation.y);
    rotation.z = glm::radians(rotation.z);
    m_Rotation = m_Rotation * glm::qua(rotation);
}

void Transform::AddScale(glm::vec3 scale)
{
    m_Scale += scale;
}

glm::vec3 Transform::GetEulerRotation() const
{
    return glm::degrees(glm::eulerAngles(m_Rotation));
}

void Transform::Serialize(Archive& ar)
{
    ar & m_Translation;
    ar & m_Rotation;
    ar & m_Scale;
}

Transform Transform::operator+(const Transform& transform) const
{
	return { m_Translation + transform.m_Translation, m_Rotation * transform.m_Rotation, m_Scale * transform.m_Scale };
}

Transform& Transform::operator=(const Transform& transform)
{
	m_Rotation = transform.m_Rotation;
	m_Scale = transform.m_Scale;
	m_Translation = transform.m_Translation;
	return *this;
}
