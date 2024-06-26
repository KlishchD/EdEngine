#include "ComponentDetailsWidget.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Components/DirectionalLightComponent.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Components/SpotLightComponent.h"
#include "Helpers/AssetHelper.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void ComponentDetailsWidget::Initialize()
{
    Widget::Initialize();

    m_AssetManager = Engine::Get().GetManager<AssetManager>();
}

void ComponentDetailsWidget::SetComponent(std::shared_ptr<Component> component)
{
    m_Component = component;
}

void ComponentDetailsWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (m_Component)
    {
        ImGui::TableNextColumn();
        static char name[1024];
        strcpy_s(name, m_Component->GetName().c_str());

        if (ImGui::InputText(("##Name" + std::to_string((int32_t)m_Component.get())).c_str(), name, 1024))
        {
            m_Component->SetName(name);
        }

        m_TransformationDetailsWidget.SetTransform(m_Component->GetRelativeTransform());
        m_TransformationDetailsWidget.Tick(DeltaTime);
        m_Component->SetRelativeTransform(m_TransformationDetailsWidget.GetTransform());
    
    
        switch (m_Component->GetType())
        {
        case ComponentType::StaticMesh:       StaticMeshDetails();       break;
        case ComponentType::PointLight:       PointLightDetails();       break;
        case ComponentType::SpotLight:        SpotLightDetails();        break;
        case ComponentType::DirectionalLight: DirectionalLightDetails(); break;
        }
    }
}

void ComponentDetailsWidget::StaticMeshDetails()
{
    std::shared_ptr<StaticMeshComponent> component = std::static_pointer_cast<StaticMeshComponent>(m_Component);
    std::shared_ptr<StaticMesh> mesh = component->GetStaticMesh();
    
    {
        std::string meshSelectionLabel = mesh ? mesh->GetName() : "Select Static Mesh";

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
                
                for (const auto& asset: m_AssetManager->GetAssets<StaticMesh>(AssetType::StaticMesh))
                {
                    if (ImGui::Selectable(AssetHelper::GetAssetNameLable(asset).c_str(), mesh == asset))
                    {
                        m_AssetManager->LoadAsset(asset->GetId());
                        component->SetStaticMesh(asset);
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
            for (std::shared_ptr<StaticSubmesh> submesh: mesh->GetSubmeshes())
            {
                {
                    std::shared_ptr<Material> material = submesh->GetMaterial();
                    std::string materialSelectionLabel = material ? material->GetName() : "Select Material";

                    ImGui::TableNextColumn();
                    ImGui::Text((submesh->GetName() + " material").c_str());
                    
                    ImGui::TableNextColumn();
                    if (ImGui::BeginCombo(("##" + submesh->GetName() + " Material").c_str(), materialSelectionLabel.data()))
                    {
                        if (ImGui::Selectable("None"))
                        {
                            submesh->SetMaterial(nullptr);
                        }
                        for (const auto& asset: m_AssetManager->GetAssets<Material>(AssetType::Material))
                        {
                            if (ImGui::Selectable(AssetHelper::GetAssetNameLable(asset).c_str(), material == asset))
                            {
                                m_AssetManager->LoadAsset(asset->GetId());
                                submesh->SetMaterial(asset);
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

void ComponentDetailsWidget::LightComponentDetails()
{
	std::shared_ptr<LightComponent> component = std::static_pointer_cast<LightComponent>(m_Component);

	if (glm::vec3 color = component->GetColor(); ImGui::ColorPicker3("Light color", glm::value_ptr(color)))
	{
		component->SetColor(color);
	}

	if (float intensity = component->GetIntensity(); ImGui::SliderFloat("Light Intensity", &intensity, 0.0f, 100000.0f))
	{
		component->SetIntensity(intensity);
	}

	if (bool casts = component->IsShadowCasting(); ImGui::Checkbox("Casts shadows", &casts))
	{
		component->SetShadowCasting(casts);
	}

	if (bool enabled = component->ShouldShowWireframe(); ImGui::Checkbox("Show wireframe", &enabled))
	{
		component->SetShowWireframe(enabled);
	}
}

void ComponentDetailsWidget::PointLightDetails()
{
    std::shared_ptr<PointLightComponent> component = std::static_pointer_cast<PointLightComponent>(m_Component);

    LightComponentDetails();

    if (float radius = component->GetRadius(); ImGui::SliderFloat("Light Radius", &radius, 0.0f, 1000.0f))
    {
        component->SetRadius(radius);
    }

    if (int32_t size = component->GetShadowFilterSize(); ImGui::SliderInt("PFC filter size", &size, 1, 64))
    {
        component->SetShadowFilterSize(size);
    }
}

void ComponentDetailsWidget::SpotLightDetails()
{
    std::shared_ptr<SpotLightComponent> component = std::static_pointer_cast<SpotLightComponent>(m_Component);

    LightComponentDetails();

    if (float angle = glm::degrees(component->GetInnerAngle()); ImGui::SliderFloat("Light inner angle", &angle, 0.0f, 89.0f))
    {
        component->SetInnerAngle(glm::radians(angle));
    }

    if (float angle = glm::degrees(component->GetOuterAngle()); ImGui::SliderFloat("Light outer angle", &angle, 0.0f, 89.0f))
    {
        component->SetOuterAngle(glm::radians(angle));
    }

    if (float distance = component->GetMaxDistance(); ImGui::SliderFloat("Light max effective distance", &distance, 0.0f, 1000.0f))
    {
        component->SetMaxDistance(distance);
    }

    if (int32_t size = component->GetShadowFilterSize(); ImGui::SliderInt("PCF filter size", &size, 1, 64))
    {
        component->SetShadowFilterSize(size);
    }

    if (float radius = component->GetShadowFilterRadius(); ImGui::SliderFloat("PCF filter radius", &radius, 1.0f, 10.0f))
    {
        component->SetShadowFilterRadius(radius);
    }
}

void ComponentDetailsWidget::DirectionalLightDetails()
{
    LightComponentDetails();

    std::shared_ptr<DirectionalLightComponent> component = std::static_pointer_cast<DirectionalLightComponent>(m_Component);

    if (int32_t count = component->GetShadowCascadesCount(); ImGui::SliderInt("CSM cascades", &count, 1, 4))
    {
        component->SetShadowCascadesCount(count);
    }

    if (float multiplier = component->GetShadowMapZMultiplier(); ImGui::SliderFloat("CSM cascade z multiplier", &multiplier, 1.0f, 100.0f, "%.10f"))
    {
        component->SetShadowMapZMultiplier(multiplier);
    }

    if (int32_t size = component->GetShadowFilterSize(); ImGui::SliderInt("PCF filter size", &size, 1, 64))
    {
        component->SetShadowFilterSize(size);
    }

    if (float radius = component->GetShadowFilterRadius(); ImGui::SliderFloat("PCF filter radius", &radius, 1.0f, 10.0f))
    {
        component->SetShadowFilterRadius(radius);
    }
}
