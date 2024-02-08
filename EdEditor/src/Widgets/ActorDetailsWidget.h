#pragma once

#include <memory>
#include "ComponentDetailsWidget.h"
#include "TransformationDetailsWidget.h"

class Actor;
class ActorDetailsWidget: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    std::shared_ptr<class Editor> m_Editor;
    
    TransformationDetailsWidget m_TransformationDetailsWidget;
    ComponentDetailsWidget m_ComponentDetailsWidget;
    
    void ComponentTree(std::shared_ptr<Actor> actor);
    void ComponentTreeRecursive(std::shared_ptr<Component> component);

    void CreateComponent(std::shared_ptr<Actor> actor);
    
};