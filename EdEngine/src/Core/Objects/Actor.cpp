#include "Actor.h"

Actor::Actor(const std::string& name): Super(name)
{
    
}

void Actor::Intialize()
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

Transform Actor::GetPreviousTransform() const
{
    return m_PreviousTransform;
}

void Actor::Update(float deltaSeconds)
{
    m_PreviousTransform = m_Transform;

    for (std::shared_ptr<Component> component : m_Components)
    {
        component->Update(deltaSeconds);
    }
}

void Actor::RegisterComponent(std::shared_ptr<Component> component)
{
    component->SetOwnerActor(shared_from_this());
    m_Components.push_back(component);
}

const std::vector<std::shared_ptr<Component>>& Actor::GetComponents() const
{
    return m_Components;
}

std::vector<std::shared_ptr<Component>> Actor::GetAllComponents() const
{
    std::vector<std::shared_ptr<Component>> components;
    for (std::shared_ptr<Component> component: m_Components)
    {
        components.push_back(component);
        for (std::shared_ptr<Component> child: component->GetAllChildren())
        {
            components.push_back(child);
        }
    }

    return components;
}

void Actor::Serialize(Archive& archive)
{
    Super::Serialize(archive);

	archive & m_Transform;
	archive & m_Components;
}
