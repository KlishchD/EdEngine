﻿#include "CubeTexture.h"

uint32_t CubeTexture::GetSize() const
{
    const CubeTextureData& data = (const CubeTextureData&) GetData();
    return data.Size;
}

CubeTexture::CubeTexture(const std::shared_ptr<CubeTextureDescriptor>& descriptor): Texture(descriptor)
{
}
