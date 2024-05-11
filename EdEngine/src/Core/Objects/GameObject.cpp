#include "GameObject.h"

GameObject::GameObject(const std::string& name)
{
    memcpy(m_Name.data(), name.data(), 1024); // TODO: What ???? Why ???
}

void GameObject::SetName(const std::string& name)
{
    memcpy(m_Name.data(), name.data(), 1024); // TODO: What ???? Why ???
}

void GameObject::Serialize(Archive& archive)
{
    Serializable::Serialize(archive);

	archive & m_Name;
}
