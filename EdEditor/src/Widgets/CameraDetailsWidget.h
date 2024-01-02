#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Core/Widget.h"

class CameraDetailsWidget: public Widget
{
    inline static int32_t InstancesCount = 0;
public:
    CameraDetailsWidget();
    
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;

private:
    std::shared_ptr<class Editor> m_Editor;
    class Engine* m_Engine = nullptr;
    std::shared_ptr<class Renderer> m_Renderer;

    bool PositionSliders(glm::vec3& position, glm::vec2 range);
    bool RotationSliders(glm::vec3& angles);
};
