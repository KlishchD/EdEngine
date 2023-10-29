#include "CubeFramebuffer.h"

#include "Core/Assets/Texture3D.h"

CubeFramebuffer::CubeFramebuffer(int32_t size): BaseFramebuffer(), m_Size(size)
{
}

void CubeFramebuffer::CreateAttachment(FramebufferAttachmentType type)
{
    Texture3DData data;
    data.Size = m_Size;
    data.Data[0] = data.Data[1] = data.Data[2] = data.Data[3] = data.Data[4] = data.Data[5] = nullptr;
    
    switch (type)
    {
    case FramebufferAttachmentType::Color:
        {
            data.Channels = 4;
            Texture3DImportParameters parameters = {  "", GL_REPEAT, GL_REPEAT, GL_REPEAT, GL_RGBA, GL_RGBA, GL_LINEAR, GL_UNSIGNED_BYTE };
            std::shared_ptr<Texture3D> attachment = std::make_shared<Texture3D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachment->GetID(), 0);

            m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
            m_Attachments.push_back(attachment);
        }
        break;
    case FramebufferAttachmentType::Depth:
        {
            data.Channels = 1;
            Texture3DImportParameters parameters = { "", GL_REPEAT, GL_REPEAT, GL_REPEAT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_NEAREST, GL_UNSIGNED_BYTE };
            m_DepthAttachment = std::make_shared<Texture3D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_DepthAttachment->GetID(), 0);
        }
        break;
    case FramebufferAttachmentType::DepthStencil:
        {
            data.Channels = 3;
            Texture3DImportParameters parameters = { "", GL_REPEAT, GL_REPEAT, GL_REPEAT, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_NEAREST, GL_UNSIGNED_INT_24_8 };
            m_DepthStencilAttachment = std::make_shared<Texture3D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_DepthStencilAttachment->GetID(), 0);
        }
        break;
    case FramebufferAttachmentType::Direction:
        {
            data.Channels = 3;
            Texture3DImportParameters parameters = {  "", GL_REPEAT, GL_REPEAT, GL_REPEAT, GL_RGBA16F, GL_RGBA, GL_LINEAR, GL_FLOAT };
            std::shared_ptr<Texture3D> attachment = std::make_shared<Texture3D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachment->GetID(), 0);

            m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
            m_Attachments.push_back(attachment);
        }
        break;
    case FramebufferAttachmentType::Position:
        {
            data.Channels = 3;
            Texture3DImportParameters parameters = {  "", GL_REPEAT, GL_REPEAT, GL_REPEAT, GL_RGBA32F, GL_RGBA, GL_LINEAR, GL_FLOAT };
            std::shared_ptr<Texture3D> attachment = std::make_shared<Texture3D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachment->GetID(), 0);

            m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
            m_Attachments.push_back(attachment);
        }
        break;

    case FramebufferAttachmentType::Distance:
        {
            data.Channels = 1;
            Texture3DImportParameters parameters = {  "", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_R32F, GL_RED, GL_LINEAR, GL_FLOAT };
            std::shared_ptr<Texture3D> attachment = std::make_shared<Texture3D>(parameters, data);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachment->GetID(), 0);

            m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
            m_Attachments.push_back(attachment);
        }
        break;
    }

    glDrawBuffers(m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());
    
    int32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    ED_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "[RendererAPI] Failed to attach texture to framebuffer")
}

void CubeFramebuffer::AttachFace(int32_t face) const
{
    for (int32_t i = 0; i < m_Attachments.size(); ++i) 
    {
        if (std::shared_ptr<Texture3D> texture3D = std::static_pointer_cast<Texture3D>(m_Attachments[i]))
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture3D->GetID(), 0);
        }
    }

    glDrawBuffers(m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());

    if (m_DepthAttachment) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_DepthAttachment->GetID(), 0);
    }
    
    if (m_DepthStencilAttachment) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_DepthStencilAttachment->GetID(), 0);
    }
}

void CubeFramebuffer::AttachLayers() const
{
	for (int32_t i = 0; i < m_Attachments.size(); ++i)
	{
		if (std::shared_ptr<Texture3D> texture3D = std::static_pointer_cast<Texture3D>(m_Attachments[i]))
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture3D->GetID(), 0);
		}
	}

	glDrawBuffers(m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());

	if (m_DepthAttachment) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetID(), 0);
	}

	if (m_DepthStencilAttachment) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthStencilAttachment->GetID(), 0);
	}
}

void CubeFramebuffer::Resize(int32_t size)
{
    m_Size = size;
    
    for (const std::shared_ptr<Texture>& attachment: m_Attachments)
    {
        attachment->Resize(size, size);
    }

    if (m_DepthAttachment)
    {
        m_DepthAttachment->Resize(size, size);
    }
}

int32_t CubeFramebuffer::GetSize() const
{
    return m_Size;
}

int32_t CubeFramebuffer::GetWidth() const
{
    return m_Size;
}

int32_t CubeFramebuffer::GetHeight() const
{
    return m_Size;
}
