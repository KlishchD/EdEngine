#pragma once

#include "TextureData.h"

class Texture final
{
    friend class RenderingContext;
public:
    void SetDebugName(ccstr8 name);
    void GetDebugName(ccstr8& name);

    void SetData(TextureData* inData, bool resize);
    void SetData(void* inData);

    void Resize(u32 width, u32 height, u32 depth);
    void Resize(glm::u32vec3 size);

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }
    TextureType GetType() const { return m_Type; }
    PixelFormat GetFormat() const { return m_Format; }

    u32 GetWidth() const { return m_Size.x; }
    u32 GetHeight() const { return m_Size.y; }
    u32 GetDepth() const { return m_Size.z; }
    glm::u32vec3 GetSize() const { return m_Size; }

    ~Texture();

protected:
    Texture(TextureType type, PixelFormat format, glm::u32vec3 size);
protected:
    uptr m_NativeHandle;
    TextureType m_Type;
    PixelFormat m_Format;
    glm::u32vec3 m_Size;
};