#include "RendererStatsWidget.h"
#include "Core/Engine.h"
#include "Core/Rendering/Renderer.h"
#include "imgui.h"

void RendererStatsWidget::Initialize()
{
    Widget::Initialize();

    m_Renderer = Engine::Get().GetRenderer();
}

void RendererStatsWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (ImGui::Begin("Renderer stats"))
    {
        ImGui::Text("Static Meshes Drawn: %d", m_Renderer->StaticMeshesDrawn);
        
        ImGui::Text("Point Lights Shaded: %d", m_Renderer->PointLightsShaded);
        ImGui::Text("Point Lights Shadows Drawn: %d", m_Renderer->PointLightsShadowsDrawn);
        
        ImGui::Text("Spot Lights Shaded: %d", m_Renderer->SpotLightsShaded);
        ImGui::Text("Spot Lights Shadows Drawn: %d", m_Renderer->SpotLightsShadowsDrawn);

        ImGui::Text("Directional Lights Shaded: %d", m_Renderer->DirectionalLightsShaded);
        ImGui::Text("Directional Lights Shadows Drawn: %d", m_Renderer->DirectionalLightsShadowsDrawn);
    }

    ImGui::End();
}
