#pragma once

#include "GameObject.h"
#include "Core/Components/Component.h"
#include "Core/Ed.h"

class Actor : public GameObject, public std::enable_shared_from_this<Actor>
{
    friend class boost::serialization::access;

    template <class Archive>
    void save(Archive& ar, const uint32_t version) const
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        ar & m_Components.size();

        ar & m_Transform;

        for (const std::shared_ptr<Component>& component: GetComponents())
        {
            ar & component.get();
        }
    }

    template <class Archive>
    void load(Archive& ar, const uint32_t version)
    {
        ar & boost::serialization::base_object<GameObject>(*this);
        
        int32_t componentsNumber;
        ar & componentsNumber;
        
        ar & m_Transform;

        for (int32_t componentIndex = 0; componentIndex < componentsNumber; ++componentIndex)
        {
            Component* component;
            ar & component;
            RegisterComponent(std::shared_ptr<Component>(component));
        }
    }
    
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    
public:
    Actor(const std::string& name = "empty");

    void SetTransform(const Transform& transform);
    Transform& GetTransform();
    Transform GetPreviousTransform() const;

    virtual void Update(float DeltaSeconds);

    void RegisterComponent(const std::shared_ptr<Component>& component);
    const std::vector<std::shared_ptr<Component>>& GetComponents() const;

    std::vector<std::shared_ptr<Component>> GetAllComponents() const;
protected:
    std::vector<std::shared_ptr<Component>> m_Components;
    Transform m_Transform;
    Transform m_PreviousTransform;
};

BOOST_CLASS_EXPORT_KEY(Actor)
BOOST_CLASS_VERSION(Actor, 1)