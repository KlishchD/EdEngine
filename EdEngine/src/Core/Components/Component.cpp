#include "Component.h"

Component::Component(const std::string& name) : m_Name(name)
{
}

ComponentType Component::GetType() const
{
    return ComponentType::Base;
}

void Component::ClearChildren()
{
    m_Children.clear();
}

void Component::AddChild(const std::shared_ptr<Component>& component)
{
    m_Children.push_back(component);
}

const std::vector<std::shared_ptr<Component>>& Component::GetChildren() const
{
    return m_Children;
}

std::vector<std::shared_ptr<Component>> Component::GetAllChildren()
{
    std::vector<std::shared_ptr<Component>> components;
    for (const std::shared_ptr<Component>& child: m_Children)
    {
        for (const std::shared_ptr<Component>& component: child->GetAllChildren())
        {
            components.push_back(component);
        }
    }
    
    return components;
}

void Component::SetTransform(const Transform& transform)
{
    m_Transform = transform;
}

Transform& Component::GetTransform()
{
    return m_Transform;
}

const std::string& Component::GetName() const
{
    return m_Name;
}

void Component::SetName(const std::string& name)
{
    m_Name = name;
}
