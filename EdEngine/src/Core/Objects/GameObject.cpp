#include "GameObject.h"

BOOST_CLASS_EXPORT_IMPLEMENT(GameObject)

GameObject::GameObject(const std::string& name)
{
    memcpy(m_Name.data(), name.data(), 1024);
}

void GameObject::SetName(const std::string& name)
{
    memcpy(m_Name.data(), name.data(), 1024);
}
