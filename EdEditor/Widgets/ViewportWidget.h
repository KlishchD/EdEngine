#pragma once

class Editor;

class ViewportWidget : public Widget
{
public:
    virtual void Tick(f32 DeltaTime) override;
private:
    
    glm::i32vec2 m_ViewportSize = { 1, 1 };
};
