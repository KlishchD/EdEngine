#pragma once

#include "Core/Widget.h"
#include "Core/Math/Transform.h"

class TransformationDetailsWidget: public Widget
{
    inline static int32_t InstancesCount = 0;
public:
    TransformationDetailsWidget();
    void SetTransform(const Transform& transform);
    Transform GetTransform();

    virtual void Tick(float DeltaTime) override;
    
private:
    Transform m_Transform;

    bool PositionSliders(glm::vec3& position, glm::vec2 range);
    bool RotationSliders(glm::vec3& angles);
    bool ScaleSliders(glm::vec3& scale, glm::vec2 range);

};
