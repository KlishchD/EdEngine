#pragma once

class Widget
{
public:
    virtual ~Widget() = default;
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void Tick(float DeltaTime);
};
