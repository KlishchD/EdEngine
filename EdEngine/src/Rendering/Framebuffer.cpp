#include "Framebuffer.h"
#include <iostream>

#include "Core/Assets/Texture2D.h"

Framebuffer::Framebuffer(uint32_t width, uint32_t height): m_Width(width), m_Height(height)
{
}

/*
void Framebuffer::AttachColorTexture(int32_t index, const std::shared_ptr<Texture3D>& texture, int32_t face)
{
    texture->Resize(m_Width, m_Height);
    texture->Bind();
    
    index = glm::clamp(index, 0, static_cast<int32_t>(m_Attachments.size()));
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture->GetID(), 0);
    if (index < m_Attachments.size())
    {
        m_Attachments[index] = texture;
    }
    else
    {
        m_Attachments.push_back(texture);
        m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + index);
    }
    
    glDrawBuffers(m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());

    int32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) // TODO: Asserts ;)
    {
        std::cout << "[RendererAPI] Failed to attach texture to framebuffer: " << status << std::endl;
    }
}

void Framebuffer::AttachDepthTexture(const std::shared_ptr<Texture3D>& texture, int32_t face)
{
    texture->Resize(m_Width, m_Height);
    texture->Bind();
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture->GetID(), 0);
    m_DepthAttachment = texture;

    int32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) // TODO: Asserts ;)
    {
        std::cout << "[RendererAPI] Failed to attach texture to framebuffer: " << status << std::endl;
    }
}
*/

void Framebuffer::CreateAttachment(FramebufferAttachmentType type)
{
    Texture2DData data;
    data.Width = m_Width;
    data.Height = m_Height;
    data.Data = nullptr;
    
    switch (type)
    {
    case FramebufferAttachmentType::Color:
        {
            data.Channels = 4;
            Texture2DImportParameters parameters = {  "", GL_REPEAT, GL_REPEAT, GL_RGBA, GL_RGBA, GL_LINEAR, GL_UNSIGNED_BYTE };
            std::shared_ptr<Texture2D> attachment = std::make_shared<Texture2D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_2D, attachment->GetID(), 0);

            m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
            m_Attachments.push_back(attachment);
        }
        break;
    case FramebufferAttachmentType::Depth:
        {
            data.Channels = 1;
            Texture2DImportParameters parameters = { "", GL_REPEAT, GL_REPEAT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_NEAREST, GL_UNSIGNED_BYTE };
            m_DepthAttachment = std::make_shared<Texture2D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetID(), 0);
        }
        break;
    case FramebufferAttachmentType::DepthStencil:
        {
            data.Channels = 3;
            Texture2DImportParameters parameters = { "", GL_REPEAT, GL_REPEAT, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_NEAREST, GL_UNSIGNED_INT_24_8 };
            m_DepthStencilAttachment = std::make_shared<Texture2D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetID(), 0);
        }
        break;
    case FramebufferAttachmentType::Direction:
        {
            data.Channels = 3;
            Texture2DImportParameters parameters = {  "", GL_REPEAT, GL_REPEAT, GL_RGBA16F, GL_RGBA, GL_LINEAR, GL_FLOAT };
            std::shared_ptr<Texture2D> attachment = std::make_shared<Texture2D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_2D, attachment->GetID(), 0);

            m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
            m_Attachments.push_back(attachment);
        }
        break;
    case FramebufferAttachmentType::Position:
        {
            data.Channels = 3;
            Texture2DImportParameters parameters = {  "", GL_REPEAT, GL_REPEAT, GL_RGBA32F, GL_RGBA, GL_LINEAR, GL_FLOAT };
            std::shared_ptr<Texture2D> attachment = std::make_shared<Texture2D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_2D, attachment->GetID(), 0);

            m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
            m_Attachments.push_back(attachment);
        }
        break;
    }
    
    glDrawBuffers(m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());
    
    int32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    ED_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "[RendererAPI] Failed to attach texture to framebuffer")
}

void Framebuffer::Resize(uint32_t width, uint32_t height)
{
    m_Width = width;
    m_Height = height;
    
    for (const std::shared_ptr<Texture>& attachment: m_Attachments)
    {
        attachment->Resize(width, height);
    }

    if (m_DepthAttachment)
    {
        m_DepthAttachment->Resize(width, height);
    }
}

int32_t Framebuffer::GetWidth() const
{
    return m_Width;
}

int32_t Framebuffer::GetHeight() const
{
    return m_Height;
}
