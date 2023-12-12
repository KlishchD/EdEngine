#include "Framebuffer.h"

Framebuffer::Framebuffer(uint32_t width, uint32_t height): m_Width(width), m_Height(height)
{
}

uint32_t Framebuffer::GetWidth() const
{
    return m_Width;
}

uint32_t Framebuffer::GetHeight() const
{
    return m_Height;
}
