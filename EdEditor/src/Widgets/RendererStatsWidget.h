#pragma once

#include "Core/Widget.h"
#include <memory>

class Renderer;

class RendererStatsWidget: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    std::shared_ptr<Renderer> m_Renderer;
};
