#pragma once

#include <memory>
#include "Core/Widget.h"
#include "TransformationDetailsWidget.h"

class Component;

class ComponentDetailsWidget: public Widget
{
public:
    virtual void Initialize() override;
    void SetComponent(std::shared_ptr<Component> component);
    virtual void Tick(float DeltaTime) override;
private:
    std::shared_ptr<class AssetManager> m_AssetManager;
    std::shared_ptr<Component> m_Component;

    TransformationDetailsWidget m_TransformationDetailsWidget;
    
    void StaticMeshDetails();
    void LightComponentDetails();
    void PointLightDetails();
    void SpotLightDetails();
    void DirectionalLightDetails();
};
