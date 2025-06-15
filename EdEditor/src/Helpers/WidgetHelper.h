#pragma once

namespace WidgetHelper
{
    bool TransformWindow(Transform& transform);
    bool PositionSliders(glm::vec3& position, glm::vec2 range);
    bool RotationSliders(glm::vec3& angles);
    bool ScaleSliders(glm::vec3& scale, glm::vec2 range);

    void FeatureDetails(StaticMeshFeature* feature);
    void FeatureDetails(PointLightFeature* feature);
    void FeatureDetails(SpotLightFeature* feature);
    void FeatureDetails(DirectionalLightFeature* feature);
    void FeatureDetails(CameraFeature* feature);
}