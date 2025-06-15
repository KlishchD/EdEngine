#include "EdOpenGL.h"
#include "Sampler.h"

Sampler::Sampler(WrapMode WrapS, WrapMode WrapT, WrapMode WrapR, FilteringMode Filter)
{
    extern uptr GetNextHandle(OpenGLHandleType type);
    m_NativeHandle = GetNextHandle(OpenGLHandleType::Sampler);

    glSamplerParameteri(m_NativeHandle, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(WrapS));
    glSamplerParameteri(m_NativeHandle, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(WrapT));
    glSamplerParameteri(m_NativeHandle, GL_TEXTURE_WRAP_R, OpenGLTypes::ConvertWrapMode(WrapR));

    glSamplerParameteri(m_NativeHandle, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(Filter));
    glSamplerParameteri(m_NativeHandle, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(Filter));
}

void Sampler::SetDebugName(ccstr8 name, i32 size)
{
    glObjectLabel(GL_SAMPLER, m_NativeHandle, size, name);
}

void Sampler::GetDebugName(ccstr8& name, i32& size)
{
    ED_ASSERT(size, "Expected size to be string capacity, e.g size > 0.");
    glGetObjectLabel(GL_SAMPLER, m_NativeHandle, size, &size, const_cast<c8*>(name));
}

Sampler::~Sampler()
{
    extern void ReturnHandle(OpenGLHandleType type, uptr Handle);
    ReturnHandle(OpenGLHandleType::Sampler, m_NativeHandle);
}
