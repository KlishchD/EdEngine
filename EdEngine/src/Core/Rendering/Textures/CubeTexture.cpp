#include "CubeTexture.h"

glm::u32vec3 CubeTexture::GetSize() const
{
    uint32_t size = GetDescriptor<CubeTextureDescriptor>()->Data.GetSize();
    return { size, size, size };
}

void CubeTexture::Resize(uint32_t width, uint32_t height, uint32_t depth)
{
    Resize(width);
}

void CubeTexture::Resize(glm::u32vec3 size)
{
    Resize(size.x);
}

CubeTexture::CubeTexture(std::shared_ptr<CubeTextureDescriptor> descriptor): Texture(descriptor)
{
}
