#include "Component.h"
#include "Core/Objects/Actor.h"

Component::Component(const std::string& name) : Super(name)
{
}

ComponentType Component::GetType() const
{
    return ComponentType::Base;
}

void Component::ClearChildren()
{
    for (std::shared_ptr<Component> component : m_Children)
    {
        component->SetOwnerActor(nullptr);
        component->SetOwnerComponent(nullptr);
    }
    m_Children.clear();
}

void Component::AddChild(std::shared_ptr<Component> component)
{
    component->SetOwnerActor(m_OwnerActor);
    component->SetOwnerComponent(m_OwnerComponent);
    m_Children.push_back(component);
}

const std::vector<std::shared_ptr<Component>>& Component::GetChildren() const
{
    return m_Children;
}

std::vector<std::shared_ptr<Component>> Component::GetAllChildren()
{
    std::vector<std::shared_ptr<Component>> components;
    for (std::shared_ptr<Component> child: m_Children)
    {
        for (std::shared_ptr<Component> component: child->GetAllChildren())
        {
            components.push_back(component);
        }
    }
    
    return components;
}

void Component::SetOwnerComponent(std::shared_ptr<Component> component)
{
    m_OwnerComponent = component;
}

std::shared_ptr<Component> Component::GetOwnerComponent() const
{
    return m_OwnerComponent;
}

void Component::SetOwnerActor(std::shared_ptr<Actor> actor)
{
    m_OwnerActor = actor;
    for (const std::shared_ptr<Component> component : GetAllChildren())
    {
        component->SetOwnerActor(m_OwnerActor);
    }
}

std::shared_ptr<Actor> Component::GetOwnerActor() const
{
    return m_OwnerActor;
}

void Component::SetRelativeTransform(const Transform& transform)
{
    m_Transform = transform;
}

Transform& Component::GetRelativeTransform()
{
    return m_Transform;
}

Transform Component::GetRelativeTransform() const
{
    return m_Transform;
}

Transform Component::GetPreviousRelativeTransform() const
{
    return m_PreviousTransform;
}

Transform Component::GetWorldTransform() const
{
    Transform transform = m_Transform;

    std::shared_ptr<Component> component = m_OwnerComponent;
    while (component)
    {
        transform = transform + component->GetRelativeTransform();
        component = component->GetOwnerComponent();
    }

    if (m_OwnerActor)
    {
        transform = transform + m_OwnerActor->GetTransform();
    }

    return transform;
}

Transform Component::GetPreviousWorldTransform() const
{
	Transform transform = m_PreviousTransform;

	std::shared_ptr<Component> component = m_OwnerComponent;
	while (component)
	{
		transform = transform + component->GetPreviousRelativeTransform();
		component = component->GetOwnerComponent();
	}

	if (m_OwnerActor)
	{
		transform = transform + m_OwnerActor->GetPreviousTransform();
	}

	return transform;
}

void Component::Update(float deltaSeconds)
{
    m_PreviousTransform = m_Transform;
}

void Component::Serialize(Archive& archive)
{
    Super::Serialize(archive);
    
    archive & m_Transform;
    archive & m_Children;
}