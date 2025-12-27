#include "EdCore.h"
#include "GameObject.h"
#include "Archive.h"

GameObject::GameObject(const std::string& name) : m_Name(name)
{
}

void GameObject::Serialize(Archive<>& archive)
{
    archive & m_Name;
}
