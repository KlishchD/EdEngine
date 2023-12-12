#include "BaseFramebuffer.h"
#include "Core/Macros.h"

std::shared_ptr<Texture> BaseFramebuffer::GetAttachment(int32_t index) const
{
    ED_ASSERT(index >= 0 && index < m_Attachments.size(), "Index must be in range from 0 to {}", m_Attachments.size())
    return m_Attachments[index];
}

std::shared_ptr<Texture> BaseFramebuffer::GetDepthAttachment() const
{
    return m_DepthAttachment;
}

std::shared_ptr<Texture> BaseFramebuffer::GetDepthStencilAttachment() const
{
    return m_DepthStencilAttachment;
}

uint32_t BaseFramebuffer::GetID() const
{
    return m_Id;
}
