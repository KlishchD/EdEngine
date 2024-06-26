#include "OpenGLFramebuffer.h"
#include "Core/Rendering/EdRendering.h"
#include "Helpers/RenderingHelper.h"
#include "Core/Macros.h"

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specification) : Framebuffer(specification)
{
	glCreateFramebuffers(1, &m_Id);
}

void OpenGLFramebuffer::AddAttachment(std::shared_ptr<Texture> attachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

	if (attachment->GetPixelFormat() == PixelFormat::Depth)
	{
		m_DepthAttachment = attachment;
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetID(), 0);
	}
	else if (attachment->GetPixelFormat() == PixelFormat::DepthStencil)
	{
		m_DepthAttachment = attachment;
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment->GetID(), 0);
	}
	else
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), attachment->GetID(), 0);

		m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
		m_Attachments.push_back(attachment);

		glDrawBuffers(m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());
	}

	int32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ED_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "[RendererAPI] Failed to attach texture to framebuffer")

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::SetAttachment(int32_t index, std::shared_ptr<Texture> attachment, FramebufferSizeAdjustmentMode mode)
{
	ED_ASSERT(index < m_Attachments.size(), "SetAttachment can only replace an attachment")

	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

	if (mode == FramebufferSizeAdjustmentMode::ResizeTextureToFramebufferSize)
	{
		attachment->Resize(m_Width, m_Height, m_Depth);
	}

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, attachment->GetID(), 0);
	m_Attachments[index] = attachment;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (mode == FramebufferSizeAdjustmentMode::ResizeFramebufferToTexutreSize)
	{
		Resize(attachment->GetSize());
	}
}

void OpenGLFramebuffer::SetDepthAttachment(std::shared_ptr<Texture> attachment, FramebufferSizeAdjustmentMode mode)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

	if (mode == FramebufferSizeAdjustmentMode::ResizeTextureToFramebufferSize)
	{
		attachment->Resize(m_Width, m_Height, m_Depth);
	}

	int32_t type = attachment->GetPixelFormat() == PixelFormat::Depth ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT;
	glFramebufferTexture(GL_FRAMEBUFFER, type, attachment->GetID(), 0);
	
	m_DepthAttachment = attachment;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (mode == FramebufferSizeAdjustmentMode::ResizeFramebufferToTexutreSize)
	{
		Resize(attachment->GetSize());
	}
}

void OpenGLFramebuffer::CopyAttachment(std::shared_ptr<Framebuffer> framebuffer, int32_t attachment)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->GetID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Id);
	glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, framebuffer->GetWidth(), framebuffer->GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void OpenGLFramebuffer::CopyDepthAttachment(std::shared_ptr<Framebuffer> framebuffer)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->GetID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Id);
	glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, framebuffer->GetWidth(), framebuffer->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

uint32_t OpenGLFramebuffer::GetWidth() const
{
	return m_Width;
}

uint32_t OpenGLFramebuffer::GetHeight() const
{
	return m_Height;
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	glDeleteFramebuffers(1, &m_Id);
}
