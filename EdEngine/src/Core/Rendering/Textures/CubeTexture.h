#pragma once

#include "Texture.h"

class CubeTexture: public Texture
{
public:
    uint32_t GetSize() const;

    virtual void Resize(uint32_t size) = 0;
protected:
    CubeTexture(std::shared_ptr<CubeTextureDescriptor> descriptor);
};
