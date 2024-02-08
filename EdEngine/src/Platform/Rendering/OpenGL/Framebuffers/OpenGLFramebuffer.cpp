#include "OpenGLFramebuffer.h"
#include "Core/Rendering/EdRendering.h"
#include "Utils/RenderingHelper.h"
#include "Core/Macros.h"

OpenGLFramebuffer::OpenGLFramebuffer(uint32_t width, uint32_t height) : Framebuffer(width, height)
{
	glCreateFramebuffers(1, &m_Id);
}

void OpenGLFramebuffer::CreateAttachment(FramebufferAttachmentType type)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

	Texture2DImportParameters parameters;

	switch (type)
	{
	case FramebufferAttachmentType::Color:        parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::RGBA8F,       FilteringMode::Linear  }; break;
	case FramebufferAttachmentType::Color16:      parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::RGBA16F,      FilteringMode::Linear  }; break;
	case FramebufferAttachmentType::Depth:        parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::Depth,        FilteringMode::Nearest }; break;
	case FramebufferAttachmentType::DepthStencil: parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::DepthStencil, FilteringMode::Nearest }; break;
	case FramebufferAttachmentType::Position:     parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RGB32F,       FilteringMode::Linear  }; break;
	case FramebufferAttachmentType::Direction:    parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RGB16F,       FilteringMode::Linear  }; break;
	case FramebufferAttachmentType::Velocity:     parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RG16F,        FilteringMode::Linear  }; break;
	case FramebufferAttachmentType::Distance:     parameters = { "", WrapMode::MirroredRepeat, WrapMode::MirroredRepeat, PixelFormat::R8F,          FilteringMode::Linear  }; break;
	case FramebufferAttachmentType::Bloom:        parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::R11G11B10F,   FilteringMode::Linear  }; break;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Unsupported framebuffer attachment type")
		break;
	}

	Texture2DData data;
	data.Width = m_Width;
	data.Height = m_Height;
	data.Data = nullptr;

	std::shared_ptr<Texture2D> attachment = RenderingHelper::CreateTexture2D(parameters, data, "framebuffer attachment");

	if (type == FramebufferAttachmentType::Depth)
	{
		m_DepthAttachment = attachment;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetID(), 0);
	}
	else if (type == FramebufferAttachmentType::DepthStencil)
	{
		m_DepthStencilAttachment = attachment;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetID(), 0);
	}
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_2D, attachment->GetID(), 0);

		m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
		m_Attachments.push_back(attachment);

		glDrawBuffers(m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());
	}

	int32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ED_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "[RendererAPI] Failed to attach texture to framebuffer")

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::SetAttachment(int32_t index, std::shared_ptr<Texture> texture, bool bResizeFramebufferToTextureSize)
{
	ED_ASSERT(index < m_Attachments.size(), "SetAttachment can only replace an attachment")

	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

	std::shared_ptr<Texture2D> castedTexture = std::static_pointer_cast<Texture2D>(texture);

	if (!bResizeFramebufferToTextureSize)
	{
		castedTexture->Resize(m_Width, m_Height);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->GetID(), 0);
	m_Attachments[index] = texture;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (bResizeFramebufferToTextureSize)
	{
		Resize(castedTexture->GetWidth(), castedTexture->GetHeight());
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

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
{
	if (m_Width != width || m_Height != height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

		m_Width = width;
		m_Height = height;

		for (int32_t i = 0; i < m_Attachments.size(); ++i)
		{
			std::shared_ptr<Texture2D> texture = std::static_pointer_cast<Texture2D>(m_Attachments[i]);
			texture->Resize(width, height);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GetID(), 0);
		}

		if (m_DepthAttachment)
		{
			std::shared_ptr<Texture2D> texture = std::static_pointer_cast<Texture2D>(m_DepthAttachment);
			texture->Resize(width, height);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetID(), 0);
		}

		if (m_DepthStencilAttachment)
		{
			std::shared_ptr<Texture2D> texture = std::static_pointer_cast<Texture2D>(m_DepthStencilAttachment);
			texture->Resize(width, height);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetID(), 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	glDeleteFramebuffers(1, &m_Id);
}
