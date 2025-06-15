#include "EdOpenGL.h"
#include "Texture.h"

Texture::Texture(TextureType type, PixelFormat format, glm::u32vec3 size) : m_Type(type), m_Format(format), m_Size(size), m_NativeHandle(0)
{
    extern uptr GetNextHandle(OpenGLHandleType type);
    
    switch (type)
    {
    case TextureType::Texture2D: m_NativeHandle = GetNextHandle(OpenGLHandleType::Texture2D); break;
    case TextureType::TextureArray: m_NativeHandle = GetNextHandle(OpenGLHandleType::TextureArray); break;
    default:
        ED_ASSERT(0, "OpenGL does not support this texture type");
    }

    Resize(m_Size);
}

void Texture::SetDebugName(ccstr8 name, i32 size)
{
    glObjectLabel(GL_TEXTURE, m_NativeHandle, size, name);
}

void Texture::GetDebugName(ccstr8& name, i32& size)
{
    ED_ASSERT(size, "Expected size to be string capacity, e.g size > 0.");
    glGetObjectLabel(GL_TEXTURE, m_NativeHandle, size, &size, const_cast<c8*>(name));
}

void Texture::SetData(TextureData* inData, bool resize)
{
    ED_ASSERT(m_NativeHandle, "Attempted to set data for uninialized texture.");
    ED_ASSERT(inData, "Attemped to set null data.");
    ED_ASSERT(inData->GetTextureType() == m_Type, "Attempted to set data of another texture type.");

    switch (m_Type)
    {
    case TextureType::Texture2D:
    {
        Texture2DData* data = (Texture2DData*)inData;

        if (resize)
        {
            Resize(data->GetWidth(), data->GetHeight(), 0);
        }

        glTextureSubImage2D(m_NativeHandle, 1, 0, 0, glm::min(m_Size.x, data->GetWidth()), glm::min(m_Size.y, data->GetHeight()), OpenGLTypes::ConvertPixelExternalFormat(m_Format), OpenGLTypes::ConvertDataType(m_Format), data->GetData());
        break;
    }
    case TextureType::TextureArray:
    {
        Texture2DArrayData* data = (Texture2DArrayData*)inData;

        if (resize)
        {
            Resize(data->GetWidth(), data->GetHeight(), data->GetDepth());
        }

        glTextureSubImage3D(m_NativeHandle, 1, 0, 0, 0, glm::min(m_Size.x, data->GetWidth()), glm::min(m_Size.y, data->GetHeight()), glm::min(m_Size.z, data->GetDepth()), OpenGLTypes::ConvertPixelExternalFormat(m_Format), OpenGLTypes::ConvertDataType(m_Format), data->GetData());
        break;
    }
    default:
        ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Texture type is not supported");
    }
}

void Texture::SetData(void* inData)
{
    ED_ASSERT(m_NativeHandle, "Attempted to set data for uninialized texture.");
    ED_ASSERT(inData, "Attemped to set null data.");

    switch (m_Type)
    {
    case TextureType::Texture2D:
    {
        glTextureSubImage2D(m_NativeHandle, 0, 0, 0, m_Size.x, m_Size.y, OpenGLTypes::ConvertPixelExternalFormat(m_Format), OpenGLTypes::ConvertDataType(m_Format), inData);
        break;
    }
    case TextureType::TextureArray:
    {
        glTextureSubImage3D(m_NativeHandle, 0, 0, 0, 0, m_Size.x, m_Size.y, m_Size.z, OpenGLTypes::ConvertPixelExternalFormat(m_Format), OpenGLTypes::ConvertDataType(m_Format), inData);
        break;
    }
    default:
        ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Texture type is not supported");
    }
}

void Texture::Resize(u32 width, u32 height, u32 depth)
{
    Resize({ width, height, depth });
}

void Texture::Resize(glm::u32vec3 size)
{
    switch (m_Type)
    {
    case TextureType::Texture2D:
    {
        glTextureStorage2D(m_NativeHandle, 1, OpenGLTypes::ConvertPixelFormat(m_Format), size.x, size.y);
        break;
    }
    case TextureType::TextureArray:
    {
        glTextureStorage3D(m_NativeHandle, 0, OpenGLTypes::ConvertPixelFormat(m_Format), size.x, size.y, size.z);
        break;
    }
    default:
        ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Texture type is not supported");
    }
}

Texture::~Texture()
{
    extern void ReturnHandle(OpenGLHandleType type, uptr Handle);

    switch (m_Type)
    {
    case TextureType::Texture2D: ReturnHandle(OpenGLHandleType::Texture2D, m_NativeHandle); break;
    case TextureType::TextureArray: ReturnHandle(OpenGLHandleType::TextureArray, m_NativeHandle); break;
    default:
        ED_ASSERT(0, "OpenGL does not support this texture type");
    }
}
