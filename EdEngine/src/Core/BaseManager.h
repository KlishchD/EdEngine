#pragma once

#include <memory>

class Engine;

class BaseManager : public std::enable_shared_from_this<BaseManager>
{
public:
    virtual ~BaseManager() = default;
    virtual void Initialize(Engine* engine) = 0;
    virtual void Deinitialize() = 0;
};
