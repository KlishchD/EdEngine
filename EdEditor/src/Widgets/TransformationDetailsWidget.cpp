#include "TransformationDetailsWidget.h"

#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

TransformationDetailsWidget::TransformationDetailsWidget()
{
    InstancesCount++;
}

void TransformationDetailsWidget::SetTransform(const Transform& transform)
{
    m_Transform = transform;
}

Transform TransformationDetailsWidget::GetTransform()
{
    return m_Transform;
}

void TransformationDetailsWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (ImGui::BeginTable("Transform", 2, ImGuiTableFlags_SizingStretchProp))
    {
        glm::vec3 translation = m_Transform.GetTranslation();
        glm::vec3 rotation = m_Transform.GetEulerRotation();
        glm::vec3 scale = m_Transform.GetScale();
        
        if (PositionSliders(translation, { -10000.0f, 10000.0f }))
        {
            m_Transform.AddTranslation(translation - m_Transform.GetTranslation());
        }
        if (RotationSliders(rotation))
        {
            m_Transform.SetRotation(glm::tquat(glm::radians(rotation)));
        }
        if (ScaleSliders(scale, { 0.00001f, 100.0f }))
        {
            m_Transform.AddScale(scale - m_Transform.GetScale());
        }
        
        ImGui::EndTable();
    }
}


bool TransformationDetailsWidget::PositionSliders(glm::vec3& position, glm::vec2 range)
{
    ImGui::TableNextColumn(); ImGui::Text("Position");
    ImGui::TableNextColumn();
    return ImGui::SliderFloat3(("##TransformationDetailsWidgetPosition" + std::to_string(InstancesCount)).data(), glm::value_ptr(position), range.x, range.y, "%.3f", ImGuiSliderFlags_Logarithmic);
}

bool TransformationDetailsWidget::RotationSliders(glm::vec3& angles)
{
    ImGui::TableNextColumn(); ImGui::Text("Rotation");
    ImGui::TableNextColumn();
    return ImGui::SliderFloat3(("##TransformationDetailsWidgetRotation" + std::to_string(InstancesCount)).data(), glm::value_ptr(angles), -180, 180);
}

bool TransformationDetailsWidget::ScaleSliders(glm::vec3& scale, glm::vec2 range)
{
    ImGui::TableNextColumn(); ImGui::Text("Scale");
    ImGui::TableNextColumn();
    return ImGui::SliderFloat3(("##TransformationDetailsWidgetScale" + std::to_string(InstancesCount)).data(), glm::value_ptr(scale), range.x, range.y);
}