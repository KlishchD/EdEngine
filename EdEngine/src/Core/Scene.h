#pragma once

#include "Core/Ed.h"
#include "Objects/Actor.h"
#include "Objects/PlayerActor.h"

// TODO : I guess in future it's better to make Scene an asset 

class Scene : public Serializable
{
public:
    Scene(std::string name = "New Scene");

    void AddActor(std::shared_ptr<Actor> actor) 
    {
        m_Actors.push_back(actor);
    }

    template <class T>
    std::shared_ptr<T> CreateActor(const std::string& name)
    {
        std::shared_ptr<T> actor = std::make_shared<T>(name);
        m_Actors.push_back(actor);
        return actor;
    }
    
    virtual void Initialize();

    virtual void Update(float deltaSeconds);

    const std::vector<std::shared_ptr<Actor>>& GetActors() const { return m_Actors; }

    std::vector<std::shared_ptr<class Component>> GetAllComponents() const;

    std::shared_ptr<PlayerActor> GetPlayerActor() const;

    virtual void Serialize(Archive& archive) override;
private:
    std::string m_Name;

	std::shared_ptr<PlayerActor> m_PlayerActor;
    std::vector<std::shared_ptr<Actor>> m_Actors; 
};