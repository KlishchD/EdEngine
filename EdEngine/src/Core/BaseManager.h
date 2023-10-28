#pragma once

class Engine;

class BaseManager
{
public:
    virtual ~BaseManager() = default;
    virtual void Initialize(Engine* engine) = 0;
    virtual void Deinitialize() = 0;
};
