#include "EdEditor.h"
#include "WidgetHelper.h"

bool WidgetHelper::TransformWindow(Transform& transform)
{
    bool result = false;

    if (ImGui::BeginTable("Transform", 2, ImGuiTableFlags_SizingStretchProp))
    {
        glm::vec3 translation = transform.GetTranslation();
        glm::vec3 rotation = transform.GetEulerRotation();
        glm::vec3 scale = transform.GetScale();

        if (PositionSliders(translation, { -10000.0f, 10000.0f }))
        {
            transform.AddTranslation(translation - transform.GetTranslation());
            result = true;
        }

        if (RotationSliders(rotation))
        {
            transform.SetRotation(glm::quat(glm::radians(rotation)));
            result = true;
        }

        if (ScaleSliders(scale, { 0.00001f, 100.0f }))
        {
            transform.AddScale(scale - transform.GetScale());
            result = true;
        }

        ImGui::EndTable();
    }

    return result;
}

bool WidgetHelper::PositionSliders(glm::vec3& position, glm::vec2 range)
{
    ImGui::TableNextColumn(); ImGui::Text("Position");
    ImGui::TableNextColumn();
    return ImGui::SliderFloat3(("##TransformationDetailsWidgetPosition" + std::to_string(reinterpret_cast<uptr>(&position))).data(), glm::value_ptr(position), range.x, range.y, "%.3f", ImGuiSliderFlags_Logarithmic);
}

bool WidgetHelper::RotationSliders(glm::vec3& angles)
{
    ImGui::TableNextColumn(); ImGui::Text("Rotation");
    ImGui::TableNextColumn();
    return ImGui::SliderFloat3(("##TransformationDetailsWidgetRotation" + std::to_string(reinterpret_cast<uptr>(&angles))).data(), glm::value_ptr(angles), -180, 180);
}

bool WidgetHelper::ScaleSliders(glm::vec3& scale, glm::vec2 range)
{
    ImGui::TableNextColumn(); ImGui::Text("Scale");
    ImGui::TableNextColumn();
    return ImGui::SliderFloat3(("##TransformationDetailsWidgetScale" + std::to_string(reinterpret_cast<uptr>(&scale))).data(), glm::value_ptr(scale), range.x, range.y);
}

void WidgetHelper::FeatureDetails(CameraFeature* feature)
{

}

void WidgetHelper::FeatureDetails(StaticMeshFeature* feature)
{
    if (feature)
    {
        if (ImGui::BeginCombo("Select mesh", feature->Mesh ? feature->Mesh->BaseAsset->Name.c_str() : "None"))
        {
            for (StaticMeshAsset* mesh : AssetManager::Get().GetStaticMeshes())
            {
                if (ImGui::Selectable(mesh->BaseAsset->Name.c_str(), feature->Mesh == mesh))
                {
                    feature->Mesh = mesh;
                    AssetManager::Get().LoadAsset(mesh->BaseAsset, true);
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Select material override", feature->MaterialOverride ? feature->MaterialOverride->BaseAsset->Name.c_str() : "None"))
        {
            for (MaterialAsset* material : AssetManager::Get().GetMaterials())
            {
                if (ImGui::Selectable(material->BaseAsset->Name.c_str(), feature->MaterialOverride == material))
                {
                    feature->MaterialOverride = material;
                    AssetManager::Get().LoadAsset(material->BaseAsset, true);
                }
            }
            
            ImGui::EndCombo();
        }
    }
}

void WidgetHelper::FeatureDetails(PointLightFeature* feature)
{
    if (feature)
    {
        ImGui::ColorPicker3("Color", glm::value_ptr(feature->Color));

        ImGui::SliderFloat("Intensity", &feature->Intensity, 0.0f, 10.0f);

        ImGui::SliderFloat("Radius", &feature->Radius, 0, 500.0f);

        i32 size = feature->ShadowFilterSize;
        if (ImGui::SliderInt("Shadow filter size", &size, 1, 5))
        {
            feature->ShadowFilterSize = size;
        }

        f32 radius = feature->ShadowFilterRadius;
        if (ImGui::SliderFloat("Shadow filter radius", &radius, 0.1f, 10.0f))
        {
            feature->ShadowFilterRadius = radius;
        }

        float near = feature->NearPlane;
        float far = feature->FarPlane;

        if (ImGui::SliderFloat("Near plane", &near, 0.001f, 10000.0f) && near <= far)
        {
            feature->NearPlane = near;
        }

        if (ImGui::SliderFloat("Far plane", &far, 0.001f, 1000.0f) && far >= near)
        {
            feature->FarPlane = far;
        }


        ImGui::Checkbox("Is shadow casting", &feature->IsShadowCasting);
        ImGui::Checkbox("Draw wire frame", &feature->DrawWireframe);
    }
}

void WidgetHelper::FeatureDetails(SpotLightFeature* feature)
{
    if (feature)
    {
        ImGui::ColorPicker3("Color", glm::value_ptr(feature->Color));

        ImGui::SliderFloat("Intensity", &feature->Intensity, 0.0f, 10.0f);

        float inner = glm::degrees(feature->InnerAngle);
        float outer = glm::degrees(feature->OuterAngle);

        if (ImGui::SliderFloat("InnerAngle", &inner, 0, 90.0f) && inner < outer)
        {
            feature->InnerAngle = glm::radians(inner);
        }

        if (ImGui::SliderFloat("OuterAngle", &outer, 0, 90.0f) && outer > inner)
        {
            feature->OuterAngle = glm::radians(outer);
        }

        float near = feature->NearPlane;
        float far = feature->FarPlane;

        if (ImGui::SliderFloat("Near plane", &near, 0.001f, 10000.0f) && near <= far)
        {
            feature->NearPlane = near;
        }

        if (ImGui::SliderFloat("Far plane", &far, 0.001f, 1000.0f) && far >= near)
        {
            feature->FarPlane = far;
        }

        ImGui::SliderFloat("MaxDistance", &feature->MaxDistance, 0, 500.0f);

        ImGui::SliderFloat("Shadow filter radius", &feature->ShadowFilterRadius, 0.5f, 10.0f);

        i32 size = feature->ShadowFilterSize;
        if (ImGui::SliderInt("Shadow filter size", &size, 1, 5))
        {
            feature->ShadowFilterSize = size;
        }

        ImGui::Checkbox("Is shadow casting", &feature->IsShadowCasting);
        ImGui::Checkbox("Draw wire frame", &feature->DrawWireframe);
    }
}

void WidgetHelper::FeatureDetails(DirectionalLightFeature* feature)
{
    if (feature)
    {
        ImGui::ColorPicker3("Color", glm::value_ptr(feature->Color));

        ImGui::SliderFloat("Intensity", &feature->Intensity, 0.0f, 10.0f);

        ImGui::SliderFloat("Shadow map Z multiplier", &feature->ShadowMapZMultiplier, 0.0f, 30.0f);

        ImGui::SliderFloat("Shadow filter radius", &feature->ShadowFilterRadius, 0.5f, 10.0f);

        i32 count = feature->CascadesCount;
        if (ImGui::SliderInt("Cascades count", &count, 1, 4))
        {
            feature->CascadesCount = count;
        }

        i32 size = feature->ShadowFilterSize;
        if (ImGui::SliderInt("Shadow filter size", &size, 1, 5))
        {
            feature->ShadowFilterSize = size;
        }

        ImGui::Checkbox("Is shadow casting", &feature->IsShadowCasting);
    }
}
