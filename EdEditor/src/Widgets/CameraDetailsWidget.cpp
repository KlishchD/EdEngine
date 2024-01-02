#include "CameraDetailsWidget.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Rendering/Renderer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

CameraDetailsWidget::CameraDetailsWidget()
{
    InstancesCount++;
}

void CameraDetailsWidget::Initialize()
{
    Widget::Initialize();

    m_Engine = &Engine::Get();
    m_Renderer = m_Engine->GetRenderer();
    m_Editor = m_Engine->GetManager<Editor>();
}

void CameraDetailsWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (Camera* camera = m_Engine->GetCamera())
    {
        ImGui::Begin("Camera");

        if (ImGui::BeginTable("Transform", 2, ImGuiTableFlags_SizingStretchProp))
        {
            glm::vec3 translation = camera->GetPosition();
            glm::vec3 rotation = camera->GetRotation();

            PositionSliders(translation, { -10000.0f, 10000.0f });
            RotationSliders(rotation);

            ImGui::EndTable();
        }

        float mSpeed = m_Editor->GetCameraSpeed();
        ImGui::SliderFloat("Camera speed", &mSpeed, 1, 10000);
        m_Editor->SetCameraSpeed(mSpeed);

        glm::vec2 rSpeed = m_Editor->GetCameraRotationSpeed();
        ImGui::SliderFloat2("Camera rotation speed", (float*)&rSpeed, 1, 10000);
        m_Editor->SetCameraRotationSpeed(rSpeed);

        ImGui::End();
    }

    ImGui::Begin("Rendering");

    if (float strength = m_Renderer->GetBloomStrength(); ImGui::SliderFloat("Bloom strength", &strength, 0.0f, 1.0f))
    {
        m_Renderer->SetBloomStrength(strength);
    }

    if (float strength = m_Renderer->GetBloomMixStrength(); ImGui::SliderFloat("Bloom mix strength", &strength, 0.0f, 1.0f))
    {
        m_Renderer->SetBloomMixStrength(strength);
    }

    if (int32_t count = m_Renderer->GetBloomDownscaleTextureCount(); ImGui::SliderInt("Downscale count", &count, 1, 8))
    {
        m_Renderer->SetBloomDownscaleTexturesCount(count);
    }

    if (bool use = m_Renderer->IsUsingLightPassASBloomBase(); ImGui::Checkbox("Use light pass as bloom base", &use))
    {
        m_Renderer->SetUseLightPassAsBoomBase(use);
    }

    if (bool use = m_Renderer->IsUsingNewBloom(); ImGui::Checkbox("Use new bloom", &use))
    {
        m_Renderer->SetUseNewBloom(use);
    }

    if (float intensity = m_Renderer->GetBloomIntensity(); ImGui::SliderFloat("Bloom intensity", &intensity, 0.0f, 1.0f))
    {
        m_Renderer->SetBloomIntensity(intensity);
    }

    if (float scale = m_Renderer->GetUpsampleScale(); ImGui::SliderFloat("Upsample scale", &scale, 0.25f, 4.0f))
    {
        m_Renderer->SetUpsampleScale(scale);
    }

    ImGui::End();
}

bool CameraDetailsWidget::PositionSliders(glm::vec3& position, glm::vec2 range)
{
    ImGui::TableNextColumn(); ImGui::Text("Position");
    ImGui::TableNextColumn();
    return ImGui::SliderFloat3(("##CameraDetailsWidgetPosition" + std::to_string(InstancesCount)).data(), glm::value_ptr(position), range.x, range.y, "%.3f", ImGuiSliderFlags_Logarithmic);
}

bool CameraDetailsWidget::RotationSliders(glm::vec3& angles)
{
    ImGui::TableNextColumn(); ImGui::Text("Rotation");
    ImGui::TableNextColumn();
    return ImGui::SliderFloat3(("##CameraDetailsWidgetRotation" + std::to_string(InstancesCount)).data(), glm::value_ptr(angles), -180, 180);
}
