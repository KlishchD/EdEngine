#include "EdEditor.h"
#include "ViewportWidget.h"
#include "RenderTarget.h"
#include "RenderGraph.h"
#include "Helpers/RenderingHelper.h"

void ViewportWidget::Tick(f32 DeltaTime)
{
    Widget::Tick(DeltaTime);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

    if (ImGui::Begin("Viewport"))
    {
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize.x != viewportSize.x || m_ViewportSize.y != viewportSize.y)
        {
            m_ViewportSize.x = static_cast<i32>(viewportSize.x);
            m_ViewportSize.y = static_cast<i32>(viewportSize.y);
            Renderer::Get().ResizeViewport(m_ViewportSize);
        }

        Editor::Get().SetViewportIsActive(ImGui::IsWindowHovered());

        // We will composite everything needed onto display buffer, 
        // so editor now should not worry about the target.

        ResourceView view = RenderingHelper::GetWhiteTexture();// Renderer::Get().GetDisplayTarget();
        
        RenderTarget* target = Renderer::Get().GetRenderGraph()->GetRenderTarget("Scene");
        if (target)
        {
            view = target->GetSRV();
        }

        ImGui::Image(view.GPUHandlePtr(), viewportSize);
    }

    ImGui::End();

    ImGui::PopStyleVar();
}
