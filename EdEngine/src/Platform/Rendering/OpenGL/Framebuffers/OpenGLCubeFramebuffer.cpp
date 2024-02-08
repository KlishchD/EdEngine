#include "Core/Rendering/Textures/CubeTexture.h"
#include "Core/Rendering/EdRendering.h"
#include "OpenGLCubeFramebuffer.h"
#include "Utils/RenderingHelper.h"
#include "Core/Macros.h"

OpenGLCubeFramebuffer::OpenGLCubeFramebuffer(int32_t size) : CubeFramebuffer(size)
{
	glCreateFramebuffers(1, &m_Id);
}

void OpenGLCubeFramebuffer::CreateAttachment(FramebufferAttachmentType type)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

	CubeTextureImportParameters parameters;

	switch (type)
	{
	case FramebufferAttachmentType::Color:        parameters = { "", WrapMode::ClampToEdge, WrapMode::ClampToEdge, PixelFormat::RGBA8F,        FilteringMode::Linear,  WrapMode::ClampToEdge }; break;
	case FramebufferAttachmentType::Color16:      parameters = { "", WrapMode::ClampToEdge, WrapMode::ClampToEdge, PixelFormat::RGBA16F,       FilteringMode::Linear,  WrapMode::ClampToEdge }; break;
	case FramebufferAttachmentType::Depth:        parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::Depth,         FilteringMode::Nearest, WrapMode::Repeat      }; break;
	case FramebufferAttachmentType::DepthStencil: parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::DepthStencil,  FilteringMode::Nearest, WrapMode::Repeat      }; break;
	case FramebufferAttachmentType::Direction:    parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::RGB8F,         FilteringMode::Linear,  WrapMode::Repeat      }; break;
	case FramebufferAttachmentType::Position:     parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::RGB32F,        FilteringMode::Linear,  WrapMode::Repeat      }; break; // TODO: check for artifacts :)
	case FramebufferAttachmentType::Distance:     parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::R8F,           FilteringMode::Linear,  WrapMode::Repeat      }; break; // TODO: check for artifacts :)
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Unsupported cube framebuffer attachment type");
		break;
	}

	CubeTextureData data;
	data.Size = m_Size;
	data.Data[0] = data.Data[1] = data.Data[2] = data.Data[3] = data.Data[4] = data.Data[5] = nullptr;

	std::shared_ptr<CubeTexture> attachment = RenderingHelper::CreateCubeTexture(parameters, data, "cubeframebuffer attachment");

	if (type == FramebufferAttachmentType::Depth)
	{
		m_DepthAttachment = attachment;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_DepthAttachment->GetID(), 0);
	}
	else if (type == FramebufferAttachmentType::DepthStencil)
	{
		m_DepthStencilAttachment = attachment;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_DepthStencilAttachment->GetID(), 0);
	}
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachment->GetID(), 0);

		m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
		m_Attachments.push_back(attachment);

		glDrawBuffers(m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());
	}

	int32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ED_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "[RendererAPI] Failed to attach texture to framebuffer")

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLCubeFramebuffer::SetAttachment(int32_t index, std::shared_ptr<Texture> texture, bool bResizeFramebufferToTextureSize)
{
	ED_ASSERT(index < m_Attachments.size(), "SetAttachment can only replace an attachment")

	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

	std::shared_ptr<CubeTexture> castedTexture = std::static_pointer_cast<CubeTexture>(texture);

	if (!bResizeFramebufferToTextureSize)
	{
		castedTexture->Resize(m_Size);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture->GetID(), 0);
	m_Attachments[index] = texture;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (bResizeFramebufferToTextureSize)
	{
		Resize(castedTexture->GetSize());
	}
}

void OpenGLCubeFramebuffer::AttachFace(int32_t face)
{
	for (int32_t i = 0; i < m_Attachments.size(); ++i)
	{
		if (std::shared_ptr<CubeTexture> texture3D = std::static_pointer_cast<CubeTexture>(m_Attachments[i]))
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture3D->GetID(), 0);
		}
	}

	if (m_DepthAttachment) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_DepthAttachment->GetID(), 0);
	}

	if (m_DepthStencilAttachment) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_DepthStencilAttachment->GetID(), 0);
	}
}

void OpenGLCubeFramebuffer::AttachLayers()
{
	for (int32_t i = 0; i < m_Attachments.size(); ++i)
	{
		if (std::shared_ptr<CubeTexture> texture3D = std::static_pointer_cast<CubeTexture>(m_Attachments[i]))
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture3D->GetID(), 0);
		}
	}

	if (m_DepthAttachment) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetID(), 0);
	}

	if (m_DepthStencilAttachment) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthStencilAttachment->GetID(), 0);
	}
}

uint32_t OpenGLCubeFramebuffer::GetWidth() const
{
	return m_Size;
}

uint32_t OpenGLCubeFramebuffer::GetHeight() const
{
	return m_Size;
}

void OpenGLCubeFramebuffer::Resize(uint32_t size)
{ // TODO: Probably should add logic to keep correct attachment type (to face or layers)
	if (m_Size != size)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

		m_Size = size;

		for (std::shared_ptr<Texture> attachment : m_Attachments)
		{
			std::static_pointer_cast<CubeTexture>(attachment)->Resize(size);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachment->GetID(), 0);
		}

		if (m_DepthAttachment)
		{
			std::static_pointer_cast<CubeTexture>(m_DepthAttachment)->Resize(size);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_DepthAttachment->GetID(), 0);
		}

		if (m_DepthStencilAttachment)
		{
			std::static_pointer_cast<CubeTexture>(m_DepthAttachment)->Resize(size);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_DepthStencilAttachment->GetID(), 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

OpenGLCubeFramebuffer::~OpenGLCubeFramebuffer()
{
	glDeleteFramebuffers(1, &m_Id);
}
