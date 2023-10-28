#pragma once
#include "BaseManager.h"

class LogManager: public BaseManager
{
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;
};
