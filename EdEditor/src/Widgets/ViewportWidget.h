#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include "Core/Widget.h"

class ViewportWidget: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    std::shared_ptr<class Editor> m_Editor;
    std::shared_ptr<class Renderer> m_Renderer;
    
    glm::i32vec2 m_ViewportSize = { 1, 1 };
};
