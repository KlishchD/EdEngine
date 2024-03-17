#pragma once

#include "Buffer.h"
#include "Core/Rendering/Resource.h"

class IndexBuffer: public Buffer, public Resource
{
public:
    virtual uint32_t GetCount() = 0;
};
