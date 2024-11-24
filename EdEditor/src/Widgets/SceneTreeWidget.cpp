#include "SceneTreeWidget.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Objects/Actor.h"
#include "Core/Scene.h"

#include <imgui.h>

void SceneTreeWidget::Initialize()
{
    Widget::Initialize();

    m_Engine = &Engine::Get();
    m_Editor = m_Engine->GetManager<Editor>();
}

void SceneTreeWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (ImGui::Begin("Scene"))
    {
        for (const auto& actor : m_Engine->GetLoadedScene()->GetActors())
        {
            int32_t pos = actor->GetName().find_first_of((char)0);
            std::string name = actor->GetName().substr(0, pos) + "##" + std::to_string((int32_t)actor.get());
            if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf))
            {
                if (ImGui::IsItemClicked())
                {
                    m_Editor->SetSelectedActor(actor);
                }
                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
}
