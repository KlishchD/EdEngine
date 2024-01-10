﻿#include "Component.h"
#include "Core/Objects/Actor.h"

Component::Component(const std::string& name) : m_Name(name)
{
}

ComponentType Component::GetType() const
{
    return ComponentType::Base;
}

void Component::ClearChildren()
{
    for (const std::shared_ptr<Component>& component : m_Children)
    {
        component->SetOwnerActor(nullptr);
        component->SetOwnerComponent(nullptr);
    }
    m_Children.clear();
}

void Component::AddChild(const std::shared_ptr<Component>& component)
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
    for (const std::shared_ptr<Component>& child: m_Children)
    {
        for (const std::shared_ptr<Component>& component: child->GetAllChildren())
        {
            components.push_back(component);
        }
    }
    
    return components;
}

void Component::SetOwnerComponent(const std::shared_ptr<Component>& component)
{
    m_OwnerComponent = component;
}

std::shared_ptr<Component> Component::GetOwnerComponent() const
{
    return m_OwnerComponent;
}

void Component::SetOwnerActor(const std::shared_ptr<Actor>& actor)
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

void Component::SetTransform(const Transform& transform)
{
    m_Transform = transform;
}

Transform& Component::GetTransform()
{
    return m_Transform;
}

Transform Component::GetPreviousTransform() const
{
    return m_PreviousTransform;
}

Transform Component::GetFullTransform() const
{
    Transform transform = m_Transform;

    std::shared_ptr<Component> component = m_OwnerComponent;
    while (component)
    {
        transform = transform + component->GetTransform();
        component = component->GetOwnerComponent();
    }

    if (m_OwnerActor)
    {
        transform = transform + m_OwnerActor->GetTransform();
    }

    return transform;
}

Transform Component::GetFullPreviousTransform() const
{
	Transform transform = m_PreviousTransform;

	std::shared_ptr<Component> component = m_OwnerComponent;
	while (component)
	{
		transform = transform + component->GetPreviousTransform();
		component = component->GetOwnerComponent();
	}

	if (m_OwnerActor)
	{
		transform = transform + m_OwnerActor->GetPreviousTransform();
	}

	return transform;
}

void Component::Update(float DeltaSeconds)
{
    m_PreviousTransform = m_Transform;
}

const std::string& Component::GetName() const
{
    return m_Name;
}

void Component::SetName(const std::string& name)
{
    m_Name = name;
}
