#pragma once

#include "Core/Ed.h"

enum class ComponentType: uint8_t
{
    Base,
    StaticMesh,
    PointLight,
    SpotLight,
    DirectionalLight
};

class Actor;

class Component
{
    friend class boost::serialization::access;

    template <class Archive>
    void save(Archive& ar, uint32_t version) const
    {
        ar & m_Name;

        ar & m_Transform;

        ar & m_Children.size();

        for (std::shared_ptr<Component> component: m_Children)
        {
            ar & *component;
        }
    }

    template <class Archive>
    void load(Archive& ar, uint32_t version)
    {
        ar & m_Name;
        
        ar& m_Transform;

        int32_t size;
        ar & size;
        
        for (int32_t i = 0; i < size; ++i)
        {
            Component* component;
            ar & component;
            m_Children.push_back(std::shared_ptr<Component>(component));
        }
    }
    
    BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
    virtual ~Component() = default;
    Component(const std::string& name = "None");

    virtual ComponentType GetType() const;
    
    void ClearChildren();
    void AddChild(std::shared_ptr<Component> component);
    const std::vector<std::shared_ptr<Component>>& GetChildren() const;
    std::vector<std::shared_ptr<Component>> GetAllChildren();

    void SetOwnerComponent(std::shared_ptr<Component> component);
    std::shared_ptr<Component> GetOwnerComponent() const;

    void SetOwnerActor(std::shared_ptr<Actor> actor);
    std::shared_ptr<Actor> GetOwnerActor() const;

    void SetRelativeTransform(const Transform& transform);
    Transform& GetRelativeTransform();
    Transform GetRelativeTransform() const;
    Transform GetPreviousRelativeTransform() const;

    Transform GetWorldTransform() const;
    Transform GetPreviousWorldTransform() const;

    virtual void Update(float DeltaSeconds);

    const std::string& GetName() const;
    void SetName(const std::string& name);
protected:
    std::string m_Name;

    std::shared_ptr<Component> m_OwnerComponent;
    std::shared_ptr<Actor> m_OwnerActor;

    Transform m_Transform;
    Transform m_PreviousTransform;
    
    std::vector<std::shared_ptr<Component>> m_Children;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Component)
BOOST_CLASS_VERSION(Component, 1)