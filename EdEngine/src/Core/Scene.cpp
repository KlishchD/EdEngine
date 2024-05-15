#include "Scene.h"

#include "Components/Component.h"
#include "Objects/Actor.h"

Scene::Scene(std::string name) : Super(name)
{
}

void Scene::Initialize()
{
	for (const std::shared_ptr<Actor>& actor : m_Actors)
	{
        if (std::shared_ptr<PlayerActor> player = std::dynamic_pointer_cast<PlayerActor>(actor))
        {
            m_PlayerActor = player;
            break;
        }
	}

    if (!m_PlayerActor)
    {
	    m_PlayerActor = std::make_shared<PlayerActor>("PlayerActor");
        m_Actors.push_back(m_PlayerActor);
    }

    for (const std::shared_ptr<Actor>& actor : m_Actors)
    {
        actor->Intialize();
    }
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
    for (const std::shared_ptr<Actor>& actor : m_Actors)
    {
        for (const std::shared_ptr<Component>& component : actor->GetAllComponents())
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

void Scene::Serialize(Archive& archive)
{
    Super::Serialize(archive);

    archive & m_Actors;
}
