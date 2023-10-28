#pragma once

#include <memory>
#include "Core/Widget.h"

class SceneTreeWidget: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    class Engine* m_Engine = nullptr;
    std::shared_ptr<class Editor> m_Editor;
};
