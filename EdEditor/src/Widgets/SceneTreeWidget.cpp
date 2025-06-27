#include "EdEditor.h"
#include "SceneTreeWidget.h"
#include "Helpers/WidgetHelper.h"

template <typename T>
void ParseFeature(T* feature, u32 type, ccstr8 typeName, Entity* entity)
{
    if (feature->OwnerEntity != entity)
    {
        return;
    }

    ImGui::PushID(static_cast<i32>(reinterpret_cast<iptr>(feature)));

    ccstr8 lable = Strings::Concat(Strings::RequestString(feature->Name, static_cast<u32>(MaxFeatureNameSize * 1.2), true), typeName);
    if (ImGui::TreeNodeEx(lable, ImGuiTreeNodeFlags_Leaf))
    {
        if (ImGui::IsItemClicked())
        {
            Editor::Get().SetSelectedEntity(feature->OwnerEntity);

            Editor::Get().SetFeatureParametersLogic([feature, type] ()
            {
                ImGui::PushID(static_cast<i32>(reinterpret_cast<iptr>(feature)));

                ImGui::Separator();

                c8* name = Memory::Get().RequestDynamicMemory<c8>(MaxFeatureNameSize, 1, "str");
                memset(name, 0, MaxFeatureNameSize);
                strncpy(name, feature->Name, MaxFeatureNameSize);

                if (ImGui::InputText("Feature name", name, MaxFeatureNameSize))
                {
                    feature->Name = name;
                }

                WidgetHelper::FeatureDetails(feature);

                ImGui::PopID();
            });
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}

void SceneTreeWidget::Tick(f32 DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (ImGui::Begin("Scene"))
    {
        if (ImGui::Button("Create entity"))
        {
            Entity* entity = EntityManager::Get().CreateEntity("DefaultName");
            Editor::Get().SetSelectedEntity(entity);
        }

        if (ImGui::CollapsingHeader("Prefab controls"))
        {
            static c8 name[1024] = { 0 };
            if (ImGui::InputText("Prefab name", name, 1024))
            {
            }

            if (ImGui::BeginCombo("##Prefab", "Select prefab to add"))
            {
                for (PrefabAsset* prefab : AssetManager::Get().GetPrefabs())
                {
                    if (ImGui::Selectable(prefab->BaseAsset->Name.c_str(), false))
                    {
                        AssetManager::Get().LoadAsset(prefab->BaseAsset);
                        EntityManager::Get().CreatePrefab(name, prefab);
                    }
                }

                ImGui::EndCombo();
            }
        }

        for (Entity* entity : EntityManager::Get().GetEntites())
        {
            if (entity->IsEditorPrefab())
            {
                continue;
            }

            if (ImGui::TreeNodeEx((entity->Name + "##" + std::to_string(reinterpret_cast<uptr>(entity))).c_str(), ImGuiTreeNodeFlags_None))
            {
                if (ImGui::IsItemClicked())
                {
                    Editor::Get().SetSelectedEntity(entity);
                }

                // TODO: This will quickly become too expensive, but it will do for now 
                ED_ITERATE_FEATURES_NAMED_PARAMS(ParseFeature, entity);
                
                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
}
