#include "EdOpenGL.h"
#include "Framebuffer.h"

Framebuffer::Framebuffer(glm::u32vec2 size) : m_Width(size.x), m_Height(size.y)
{
    extern uptr GetNextHandle(OpenGLHandleType type);
	m_NativeHandle = GetNextHandle(OpenGLHandleType::Framebuffer);
}

void Framebuffer::SetDebugName(ccstr8 name, i32 size)
{
    glObjectLabel(GL_FRAMEBUFFER, m_NativeHandle, size, name);
}

void Framebuffer::GetDebugName(ccstr8& name, i32& size)
{
    ED_ASSERT(size, "Expected size to be string capacity, e.g size > 0.");
    glGetObjectLabel(GL_FRAMEBUFFER, m_NativeHandle, size, &size, const_cast<c8*>(name));
}

Texture* Framebuffer::GetAttachment(u32 index) const
{
	ED_ASSERT(index >= 0 && index < m_Attachments.size(), "Index must be in range from 0 to {}", m_Attachments.size());
    return m_Attachments[index];
}

Texture* Framebuffer::GetDepthAttachment() const
{
    return m_DepthAttachment;
}

void Framebuffer::AddAttachment(Texture* attachment)
{
	ED_ASSERT(attachment->GetType() == TextureType::Texture2D, "Framebuffer supports only 2D textures");

	if (attachment->GetWidth() != m_Width || attachment->GetHeight() != m_Height)
	{
		attachment->Resize(m_Width, m_Height, 0);
	}

	if (attachment->GetFormat() == PixelFormat::Depth)
	{
		m_DepthAttachment = attachment;
		glNamedFramebufferTexture(m_NativeHandle, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetNativeHandle(), 0);
	}
	else if (attachment->GetFormat() == PixelFormat::DepthStencil)
	{
		m_DepthAttachment = attachment;
		glNamedFramebufferTexture(m_NativeHandle, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment->GetNativeHandle(), 0);
	}
	else
	{
		glNamedFramebufferTexture(m_NativeHandle, GL_COLOR_ATTACHMENT0 + m_Attachments.size(), attachment->GetNativeHandle(), 0);

		m_AttachmentsNames.push_back(GL_COLOR_ATTACHMENT0 + m_Attachments.size());
		m_Attachments.push_back(attachment);

		glNamedFramebufferDrawBuffers(m_NativeHandle, m_AttachmentsNames.size(), (GLenum*)m_AttachmentsNames.data());
	}

	i32 status = glCheckNamedFramebufferStatus(m_NativeHandle, GL_FRAMEBUFFER);
	ED_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "[RendererAPI] Failed to attach texture to framebuffer")
}

void Framebuffer::SetAttachment(u32 index, Texture* attachment, FramebufferSizeAdjustmentMode mode)
{
	ED_ASSERT(index < m_Attachments.size(), "SetAttachment can only replace an attachment")

	if (mode == FramebufferSizeAdjustmentMode::ResizeTextureToFramebufferSize)
	{
		attachment->Resize(m_Width, m_Height, 0);
	}

	glNamedFramebufferTexture(m_NativeHandle, GL_COLOR_ATTACHMENT0 + index, attachment->GetNativeHandle(), 0);
	m_Attachments[index] = attachment;

	if (mode == FramebufferSizeAdjustmentMode::ResizeFramebufferToTexutreSize)
	{
		Resize(attachment->GetWidth(), attachment->GetHeight());
	}
}

void Framebuffer::SetDepthAttachment(Texture* attachment, FramebufferSizeAdjustmentMode mode)
{
	if (mode == FramebufferSizeAdjustmentMode::ResizeTextureToFramebufferSize)
	{
		attachment->Resize(m_Width, m_Height, 0);
	}

	i32 type = attachment->GetFormat() == PixelFormat::Depth ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT;
	glNamedFramebufferTexture(m_NativeHandle, type, attachment->GetNativeHandle(), 0);
	
	m_DepthAttachment = attachment;

	if (mode == FramebufferSizeAdjustmentMode::ResizeFramebufferToTexutreSize)
	{
		Resize(attachment->GetSize());
	}
}

void Framebuffer::Copy(Framebuffer* framebuffer, bool colors, bool depth, bool stencil)
{
	u32 mask = 0;
	if (colors) mask |= GL_COLOR_BUFFER_BIT;
	if (depth) mask |= GL_DEPTH_BUFFER_BIT;
	if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
	glBlitNamedFramebuffer(framebuffer->GetNativeHandle(), m_NativeHandle, 0, 0, framebuffer->GetWidth(), framebuffer->GetHeight(), 0, 0, GetWidth(), GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

bool Framebuffer::Resize(u32 width, u32 height)
{
    if (m_Width != width || m_Height != height)
    {
        m_Width = width;
        m_Height = height;

        for (Texture* attachment : m_Attachments)
        {
            attachment->Resize(width, height, 0);
        }

        if (m_DepthAttachment)
        {
            m_DepthAttachment->Resize(width, height, 0);
        }

        return true;
    }

    return false;
}

bool Framebuffer::Resize(glm::u32vec2 size)
{
    return Resize(size.x, size.y);
}

Framebuffer::~Framebuffer()
{
    extern void ReturnHandle(OpenGLHandleType type, uptr Handle);
	ReturnHandle(OpenGLHandleType::Framebuffer, m_NativeHandle);
}
