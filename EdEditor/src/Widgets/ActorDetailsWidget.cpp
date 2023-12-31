﻿#include "ActorDetailsWidget.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Core/Objects/Actor.h"

#include <imgui.h>

void ActorDetailsWidget::Initialize()
{
    Widget::Initialize();

    m_Editor = Engine::Get().GetManager<Editor>();

    m_ComponentDetailsWidget.Initialize();
}

void ActorDetailsWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (std::shared_ptr<Actor> actor = m_Editor->GetSelectedActor())
    {
        ImGui::Begin("Details");

        if (ImGui::BeginTable("##Details", 1, ImGuiTableFlags_BordersInnerH))
        {
            ImGui::TableNextColumn();
            ImGui::InputText("##Name", actor->GetName().data(), 1024);
            
            ImGui::TableNextColumn(); ComponentTree(actor);
            ImGui::TableNextColumn(); CreateComponent(actor);


            m_TransformationDetailsWidget.SetTransform(actor->GetTransform());
            ImGui::TableNextColumn(); m_TransformationDetailsWidget.Tick(DeltaTime);
            actor->SetTransform(m_TransformationDetailsWidget.GetTransform());

            m_ComponentDetailsWidget.SetComponent(m_Editor->GetSelectedComponent());
            ImGui::TableNextColumn(); m_ComponentDetailsWidget.Tick(DeltaTime);

            ImGui::EndTable();
        }

        ImGui::End();
    }
}

void ActorDetailsWidget::ComponentTree(const std::shared_ptr<Actor>& actor)
{
    for (const std::shared_ptr<Component>& component: actor->GetComponents())
    {
        ComponentTreeRecursive(component);
    }
}

void ActorDetailsWidget::ComponentTreeRecursive(const std::shared_ptr<Component>& component)
{
    if (ImGui::TreeNodeEx(component->GetName().data(), component->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None))
    {
        if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
        {
            m_Editor->SetSelectedComponent(component);
        }
        
        for (const std::shared_ptr<Component>& child: component->GetChildren())
        {
            ComponentTreeRecursive(child);
        }
        
        ImGui::TreePop();
    }
}

void ActorDetailsWidget::CreateComponent(const std::shared_ptr<Actor>& actor)
{
    if (ImGui::BeginCombo("Add component", "Select type"))
    {
        if (ImGui::Selectable("Static Mesh"))
        {
            actor->RegisterComponent(std::make_shared<StaticMeshComponent>());
        }

        if (ImGui::Selectable("Point Light"))
        {
            actor->RegisterComponent(std::make_shared<PointLightComponent>());
        }
                
        ImGui::EndCombo();
    }
}
