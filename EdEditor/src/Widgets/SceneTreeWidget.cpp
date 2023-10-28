#include "SceneTreeWidget.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Objects/Actor.h"

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

    ImGui::Begin("Scene");

    for (const auto& actor: m_Engine->GetLoadedScene()->GetActors())
    {
        if (ImGui::TreeNodeEx(actor->GetName().c_str(), ImGuiTreeNodeFlags_Leaf))
        {
            if (ImGui::IsItemClicked())
            {
                m_Editor->SetSelectedActor(actor);
            }
            ImGui::TreePop();
        }
    }
        
    ImGui::End();
}
