#pragma once

#include "Texture.h"

class CubeTexture : public Texture
{
public:
    glm::u32vec3 GetSize() const override;

    virtual void Resize(uint32_t width, uint32_t height, uint32_t depth) override;
    virtual void Resize(glm::u32vec3 size) override;
    virtual void Resize(uint32_t size) = 0;
protected:
    CubeTexture(std::shared_ptr<CubeTextureDescriptor> descriptor);
};
