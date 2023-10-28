#pragma once

#include "Asset.h"

class Texture : public Asset {
public:
    Texture();
	
    void Bind(uint32_t slot = 0);
    void Unbind();

    int32_t GetTextureType() const { return m_TextureType; }
    
    int32_t GetWidth() const { return m_Width; }
    int32_t GetHeight() const { return m_Height; }
	
    uint32_t GetID() const;
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    virtual ~Texture();
protected:
    uint32_t m_Id;
    int32_t m_Width;
    int32_t m_Height;
    int32_t m_TextureType;
};