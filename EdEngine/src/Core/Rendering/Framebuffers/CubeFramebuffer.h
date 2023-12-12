#pragma once

#include "BaseFramebuffer.h"

class CubeFramebuffer: public BaseFramebuffer
{
public:
    CubeFramebuffer(int32_t size);

    virtual void AttachFace(int32_t face) = 0;
    virtual void AttachLayers() = 0;

    virtual void Resize(uint32_t size) = 0;

    int32_t GetSize() const;
protected:
    int32_t m_Size;
};
