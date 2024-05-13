#include "GameObject.h"

GameObject::GameObject(const std::string& name) : m_Name(name)
{
}

void GameObject::SetName(const std::string& name)
{
    m_Name = name;
}

void GameObject::Serialize(Archive& archive)
{
    Serializable::Serialize(archive);

	archive & m_Name;
}
