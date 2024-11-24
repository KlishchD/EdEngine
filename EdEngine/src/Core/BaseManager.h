#pragma once

#include <memory>
#include "Core/Objects/GameObject.h"

ED_ABSTRACT_CLASS(BaseManager) : public GameObject, public std::enable_shared_from_this<BaseManager>
{
    ED_CLASS_BODY(BaseManager, GameObject)
public:
    virtual ~BaseManager() = default;
    virtual void Initialize(Engine* engine) = 0;
    virtual void Deinitialize() = 0;
    virtual void Update(float DeltaSeconds);
};
