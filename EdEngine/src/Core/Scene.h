#pragma once

#include "Core/Ed.h"
#include "Objects/Actor.h"
#include "Objects/PlayerActor.h"

class Scene
{
    friend class boost::serialization::access;
    
    template <class Archive>
    void save(Archive& ar, const uint32_t version) const
    {
        ar & m_Name;
        ar & m_Actors.size();
    
        for (const std::shared_ptr<Actor> actor: m_Actors)
        {
            ar & *actor;
        }
    }

    template <class Archive>
    void load(Archive& ar, const uint32_t version)
    {
        ar & m_Name;

        int32_t actorsNumber;
        ar & actorsNumber;

        for (int32_t actorIndex = 0; actorIndex < actorsNumber; ++actorIndex)
        {
            std::shared_ptr<Actor> actor = std::make_shared<Actor>("");
            ar & *actor;
            m_Actors.push_back(actor);
        }
    }
    
    BOOST_SERIALIZATION_SPLIT_MEMBER()
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
    
    virtual void Update(float deltaSeconds);

    const std::vector<std::shared_ptr<Actor>>& GetActors() const { return m_Actors; }

    std::vector<std::shared_ptr<class Component>> GetAllComponents() const;

    std::shared_ptr<PlayerActor> GetPlayerActor() const;
private:
    std::string m_Name;

	std::shared_ptr<PlayerActor> m_PlayerActor;
    std::vector<std::shared_ptr<Actor>> m_Actors; 
};

BOOST_CLASS_VERSION(Scene, 1)