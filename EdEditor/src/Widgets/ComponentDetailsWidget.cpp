#include "ComponentDetailsWidget.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/PointLightComponent.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void ComponentDetailsWidget::Initialize()
{
    Widget::Initialize();

    m_AssetManager = Engine::Get().GetManager<AssetManager>();
}

void ComponentDetailsWidget::SetComponent(const std::shared_ptr<Component>& component)
{
    m_Component = component;
}

void ComponentDetailsWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (m_Component)
    {
        m_TransformationDetailsWidget.SetTransform(m_Component->GetTransform());
        m_TransformationDetailsWidget.Tick(DeltaTime);
        m_Component->SetTransform(m_TransformationDetailsWidget.GetTransform());
    
    
        switch (m_Component->GetType())
        {
        case ComponentType::StaticMesh: StaticMeshDetails(); break;
        case ComponentType::PointLight: PointLightDetails(); break;
        }
    }
}

void ComponentDetailsWidget::StaticMeshDetails()
{
    std::shared_ptr<StaticMeshComponent> component = std::static_pointer_cast<StaticMeshComponent>(m_Component);
    std::shared_ptr<StaticMesh> mesh = component->GetStaticMesh();
    
    {
        std::string meshSelectionLabel = mesh ? mesh->GetAssetName() : "Select Static Mesh";

        if (ImGui::BeginTable("##StaticMesh", 2))
        {
            ImGui::TableNextColumn(); ImGui::Text("Static Mesh");
            
            ImGui::TableNextColumn();
            if (ImGui::BeginCombo("StaticMesh", meshSelectionLabel.data()))
            {
                if (ImGui::Selectable("None"))
                {
                    component->SetStaticMesh(nullptr);
                }
                
                for (const auto& descriptor: m_AssetManager->GetMeshDescriptors())
                {
                    if (ImGui::Selectable(descriptor->AssetName.data(), mesh && mesh->GetDescriptor() == descriptor))
                    {
                        std::shared_ptr<StaticMesh> newStaticMesh = m_AssetManager->LoadMesh(descriptor);
                        component->SetStaticMesh(newStaticMesh);
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::EndTable();
        }
    }

    if (mesh)
    {
        if (ImGui::BeginTable("Static mesh parameters", 2))
        {
            for (const std::shared_ptr<StaticSubmesh>& submesh: mesh->GetSubmeshes())
            {
                {
                    std::shared_ptr<Material> material = submesh->GetMaterial();
                    std::string materialSelectionLabel = material ? material->GetAssetName() : "Select Material";

                    ImGui::TableNextColumn();
                    ImGui::Text((submesh->GetName() + " material").c_str());
                    
                    ImGui::TableNextColumn();
                    if (ImGui::BeginCombo(("##" + submesh->GetName() + " Material").c_str(), materialSelectionLabel.data()))
                    {
                        if (ImGui::Selectable("None"))
                        {
                            submesh->SetMaterial(nullptr);
                        }
                        for (const auto& descriptor: m_AssetManager->GetMaterialDescriptors())
                        {
                            if (ImGui::Selectable(descriptor->AssetName.data(), material && material->GetDescriptor() == descriptor))
                            {
                                std::shared_ptr<Material> newMaterial = m_AssetManager->LoadMaterial(descriptor);
                                submesh->SetMaterial(newMaterial);
                            }
                        }
                        
                        ImGui::EndCombo();
                    }
                }
            }

            ImGui::EndTable();
        }
    }

    if (ImGui::BeginTable("Add child components", 2))
    {
        ImGui::TableNextColumn(); ImGui::Text("Add child component");

        ImGui::TableNextColumn();
        if (ImGui::BeginCombo("##Add child component", "Select type"))
        {
            if (ImGui::Selectable("Static Mesh##child"))
            {
                component->AddChild(std::make_shared<StaticMeshComponent>());
            }

            if (ImGui::Selectable("Point Light##child"))
            {
                component->AddChild(std::make_shared<PointLightComponent>());
            }
            
            ImGui::EndCombo();
        }
        
        ImGui::EndTable();
    }
}

void ComponentDetailsWidget::PointLightDetails()
{
    std::shared_ptr<PointLightComponent> component = std::static_pointer_cast<PointLightComponent>(m_Component);

    float intensity = component->GetIntensity();
    if (ImGui::SliderFloat("Light Intensity", &intensity, 0.0f, 100000.0f))
    {
        component->SetIntensity(intensity);
    }
    
    glm::vec3 color = component->GetColor();
    if (ImGui::ColorPicker3("Light color", glm::value_ptr(color)))
    {
        component->SetColor(color);
    }
}
