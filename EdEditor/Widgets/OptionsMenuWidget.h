#pragma once

#include "EdEditor.h"

struct TextureAsset;

class OptionsMenuWidget : public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(f32 DeltaTime) override;
private:
    TextureAsset* m_Icon;
};
