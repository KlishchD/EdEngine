#pragma once

#include "Buffer.h"

class IndexBuffer: public Buffer
{
public:
    virtual uint32_t GetCount() = 0;
};
