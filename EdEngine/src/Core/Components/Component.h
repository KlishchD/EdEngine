#pragma once

#include "Core/Ed.h"
#include "Core/Objects/GameObject.h"
#include "Core/Math/Transform.h"

enum class ComponentType: uint8_t
{
    Base,
    StaticMesh,
    PointLight,
    SpotLight,
    DirectionalLight
};

class Actor;

ED_CLASS(Component) : public GameObject
{
    ED_CLASS_BODY(Component, GameObject)
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

    virtual void Update(float deltaSeconds);

    virtual void Serialize(Archive& archive) override;
protected:
    std::shared_ptr<Component> m_OwnerComponent;
    std::shared_ptr<Actor> m_OwnerActor;

    Transform m_Transform;
    Transform m_PreviousTransform;
    
    std::vector<std::shared_ptr<Component>> m_Children;
};
