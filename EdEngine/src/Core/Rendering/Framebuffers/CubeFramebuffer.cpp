#include "CubeFramebuffer.h"

CubeFramebuffer::CubeFramebuffer(int32_t size): m_Size(size)
{
}

int32_t CubeFramebuffer::GetSize() const
{
    return m_Size;
}