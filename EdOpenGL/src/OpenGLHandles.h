#pragma once

enum class OpenGLHandleType
{
    Texture2D,
    TextureArray,
    Sampler,
    Buffer,
    Framebuffer,
    Shader,

    Count
};

template <uint32_t Count, OpenGLHandleType Type>
class OpenGLNativeHandlesPool
{
public:
    void Initialize()
    {
        m_Handles = Memory::Get().RequestStaticMemory<u32>(Count, "OpenGLHandles");

        switch (Type)
        {
        case OpenGLHandleType::Texture2D: glCreateTextures(GL_TEXTURE_2D, Count, m_Handles); break;
        case OpenGLHandleType::TextureArray: glCreateTextures(GL_TEXTURE_2D_ARRAY, Count, m_Handles); break;
        case OpenGLHandleType::Sampler: glCreateSamplers(Count, m_Handles); break;
        case OpenGLHandleType::Buffer: glCreateBuffers(Count, m_Handles); break;
        case OpenGLHandleType::Framebuffer: glCreateFramebuffers(Count, m_Handles); break;
        default:
            // Shaders are not allocated in bulk, so they are not accounted here for 
            ED_ASSERT(0, "Object collection doesn't support provided type");
        }
    }

    uptr GetNextHandle()
    {
        ED_ASSERT(m_Handles, "OpenGL object collection was not yet initialized.");
        ED_ASSERT(m_FirstFreeHandleIndex < Count || !m_FreedHandles.IsEmpty(), "Exhausted all handels in in a pool.");

        uptr handle;

        if (m_FreedHandles.GetSize())
        {
            handle = m_FreedHandles.GetLast();
            m_FreedHandles.SwapRemove(m_FreedHandles.GetSize() - 1);
        }
        else
        {
            handle = *(m_Handles + m_FirstFreeHandleIndex);
            m_FirstFreeHandleIndex++;
        }

        return handle;
    }

    void ReturnHandle(uptr handle)
    {
        m_FreedHandles.Add(handle);
    }

protected:
    u32* m_Handles = nullptr;
    u32 m_FirstFreeHandleIndex = 0;

    Array<uptr> m_FreedHandles;
};