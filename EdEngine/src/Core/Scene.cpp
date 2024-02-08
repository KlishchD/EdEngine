﻿#include "Scene.h"

#include "Components/Component.h"
#include "Objects/Actor.h"

void Scene::Update(float DeltaSeconds)
{
    for (std::shared_ptr<Actor> actor : m_Actors)
    {
        actor->Update(DeltaSeconds);
    }
}

std::vector<std::shared_ptr<Component>> Scene::GetAllComponents() const
{
    std::vector<std::shared_ptr<Component>> components;
    for (std::shared_ptr<Actor> actor : m_Actors)
    {
        for (std::shared_ptr<Component> component : actor->GetAllComponents())
        {
            components.push_back(component);
        }
    }
    
    return components;
}
