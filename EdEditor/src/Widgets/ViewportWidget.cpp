#include "ViewportWidget.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Rendering/Renderer.h"
#include <imgui.h>

void ViewportWidget::Initialize()
{
    Widget::Initialize();

    Engine& engine = Engine::Get();
    m_Editor = engine.GetManager<Editor>();
    m_Renderer = engine.GetRenderer();
}

void ViewportWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f});

    if (ImGui::Begin("Viewport"))
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize.x != viewportSize.x || m_ViewportSize.y != viewportSize.y)
        {
            m_ViewportSize.x = static_cast<int32_t>(viewportSize.x);
            m_ViewportSize.y = static_cast<int32_t>(viewportSize.y);
            m_Renderer->ResizeViewport(m_ViewportSize);
        }

        //m_Editor->SetViewportIsActive(ImGui::IsWindowHovered());

        ImGui::Image(m_Renderer->GetViewportTexture()->GetID(), viewportSize, { 0, 1 }, { 1, 0 });
    }

    ImGui::End();

    ImGui::PopStyleVar();
}
