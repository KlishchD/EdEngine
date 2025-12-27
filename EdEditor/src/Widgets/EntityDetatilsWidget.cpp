#include "EdEditor.h"
#include "EntityDetatilsWidget.h"
#include "Helpers\WidgetHelper.h"

void EntityDetailsWidget::Tick(f32 DeltaTime)
{
    Widget::Tick(DeltaTime);

    Entity* entity = Editor::Get().GetSelectedEntity();
    if (entity)
    {
        if (ImGui::Begin("Entity details"))
        {
            static c8 name[1024];
            strcpy(name, entity->Name.c_str());

            if (ImGui::InputText("Name: ", name, 1024))
            {
                entity->Name = name;
            }

            if (ImGui::BeginCombo("Add feature", "Select type"))
            {
                if (ImGui::Selectable("Static feature", false, entity->HasPosition() ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None))
                {
                    EntityManager::Get().AttachStaticFeature(entity, nullptr);
                }

                if (ImGui::Selectable("Movable feature", false, entity->HasPosition() ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None))
                {
                    EntityManager::Get().AttachMovableFeature(entity, (StaticFeature*)nullptr);
                }

                if (ImGui::Selectable("Point light feature", false, entity->HasPosition() ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
                {
                    EntityManager::Get().AttachPointLightFeature(entity);
                }

                if (ImGui::Selectable("Spot light feature", false, entity->HasPosition() ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
                {
                    EntityManager::Get().AttachSpotLightFeature(entity);
                }

                if (ImGui::Selectable("Directional light feature", false, ImGuiSelectableFlags_None))
                {
                    EntityManager::Get().AttachDirectionalLightFeature(entity);
                }

                if (ImGui::Selectable("Camera feature", false, entity->HasPosition() ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
                {
                    EntityManager::Get().AttachCameraFeature(entity);
                }

                if (ImGui::Selectable("Static mesh feature", false, entity->HasPosition() ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled))
                {
                    EntityManager::Get().AttachStaticMeshFeature(entity, nullptr, nullptr);
                }

                ImGui::EndCombo();
            }


            if (StaticFeature* feature = entity->PositionFeature)
            {
                ImGui::Text("This entity has position in world.");
                if (WidgetHelper::TransformWindow(feature->WorldTransformation))
                {
                    if (entity->IsStaticEntity())
                    {
                        feature->ForceUpdate();
                    }
                }
            }
        }

        if (auto func = Editor::Get().GetFeatureParametersLogic())
        {
            func();
        }

        ImGui::End();
    }
}