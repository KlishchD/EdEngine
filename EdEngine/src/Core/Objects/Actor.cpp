#include "Actor.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Actor)

Actor::Actor(const std::string& name): GameObject(name)
{
    
}

void Actor::SetTransform(const Transform& transform)
{
    m_Transform = transform;
}

Transform& Actor::GetTransform()
{
    return m_Transform;
}

void Actor::RegisterComponent(const std::shared_ptr<Component>& component)
{
    m_Components.push_back(component);
}

const std::vector<std::shared_ptr<Component>>& Actor::GetComponents() const
{
    return m_Components;
}

std::vector<std::shared_ptr<Component>> Actor::GetAllComponents() const
{
    std::vector<std::shared_ptr<Component>> components;
    for (const std::shared_ptr<Component>& component: m_Components)
    {
        components.push_back(component);
        for (const std::shared_ptr<Component>& child: component->GetAllChildren())
        {
            components.push_back(child);
        }
    }

    return components;
}
