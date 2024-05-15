#pragma once

#include "GameObject.h"
#include "Core/Components/Component.h"

ED_CLASS(Actor) : public GameObject, public std::enable_shared_from_this<Actor>
{
    ED_CLASS_BODY(Actor, GameObject)
public:
    Actor(const std::string& name = "empty");

    virtual void Intialize(); 

    void SetTransform(const Transform& transform);
    Transform& GetTransform();
    Transform GetPreviousTransform() const;

    virtual void Update(float deltaSeconds);

    void RegisterComponent(std::shared_ptr<Component> component);
    const std::vector<std::shared_ptr<Component>>& GetComponents() const;

    std::vector<std::shared_ptr<Component>> GetAllComponents() const;

    virtual void Serialize(Archive& archive) override;
protected:
    std::vector<std::shared_ptr<Component>> m_Components;
    Transform m_Transform;
    Transform m_PreviousTransform;
};