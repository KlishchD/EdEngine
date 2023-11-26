#include "BaseFramebuffer.h"
#include "Core/Macros.h"

BaseFramebuffer::BaseFramebuffer()
{
    glGenFramebuffers(1, &m_Id);
}

void BaseFramebuffer::CopyAttachment(const std::shared_ptr<BaseFramebuffer>& framebuffer, int32_t attachment)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->GetId());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Id);
    glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, framebuffer->GetWidth(), framebuffer->GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void BaseFramebuffer::CopyDepthAttachment(const std::shared_ptr<BaseFramebuffer>& framebuffer)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->GetId());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Id);
    glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, framebuffer->GetWidth(), framebuffer->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

std::shared_ptr<Texture> BaseFramebuffer::GetAttachment(int32_t index) const
{
    ED_ASSERT(index >= 0 && index < m_Attachments.size(), "Index must be in range from 0 to {}", m_Attachments.size())
    return m_Attachments[index];
}

std::shared_ptr<Texture> BaseFramebuffer::GetDepthAttachment() const
{
    return m_DepthAttachment;
}

void BaseFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
    glViewport(0, 0, GetWidth(), GetHeight());
}

void BaseFramebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int32_t BaseFramebuffer::GetId() const
{
    return m_Id;
}

void BaseFramebuffer::Resize(uint32_t size)
{
    Resize(size, size);
}

void BaseFramebuffer::Resize(uint32_t width, uint32_t height)
{
}
