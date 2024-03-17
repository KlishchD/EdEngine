#include "Scene.h"

#include "Components/Component.h"
#include "Objects/Actor.h"

Scene::Scene(std::string name)
{
    m_Name = name;
    m_PlayerActor = std::make_shared<PlayerActor>("PlayerActor");
}

void Scene::Update(float deltaSeconds)
{
    for (std::shared_ptr<Actor> actor : m_Actors)
    {
        actor->Update(deltaSeconds);
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

std::shared_ptr<PlayerActor> Scene::GetPlayerActor() const
{
    return m_PlayerActor;
}
