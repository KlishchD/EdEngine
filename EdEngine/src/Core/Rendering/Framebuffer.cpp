#include "Framebuffer.h"
#include "Core/Macros.h"
#include "Framebuffer.h"

Framebuffer::Framebuffer(uint32_t width, uint32_t height, uint32_t depth) : m_Width(width), m_Height(height), m_Depth(depth)
{
}

std::shared_ptr<Texture> Framebuffer::GetAttachment(int32_t index) const
{
    ED_ASSERT(index >= 0 && index < m_Attachments.size(), "Index must be in range from 0 to {}", m_Attachments.size())
    return m_Attachments[index];
}

std::shared_ptr<Texture> Framebuffer::GetDepthAttachment() const
{
    return m_DepthAttachment;
}

uint32_t Framebuffer::GetID() const
{
    return m_Id;
}

void Framebuffer::Resize(uint32_t width, uint32_t height, uint32_t depth)
{
	if (m_Width != width || m_Height != height || m_Depth != depth)
	{
		m_Width = width;
		m_Height = height;
		m_Depth = depth;

		for (std::shared_ptr<Texture> attachment : m_Attachments)
		{
			attachment->Resize(width, height, depth);
		}

		if (m_DepthAttachment)
		{
			m_DepthAttachment->Resize(width, height, depth);
		}
	}
}

void Framebuffer::Resize(glm::u32vec3 size)
{
	Resize(size.x, size.y, size.z);
}

uint32_t Framebuffer::GetWidth() const
{
	return m_Width;
}

uint32_t Framebuffer::GetHeight() const
{
	return m_Height;
}

uint32_t Framebuffer::GetDepth() const
{
	return m_Depth;
}

void Framebuffer::SetActiveViewportPercentage(float percentage)
{
	m_ActiveViewportPercentage = percentage;

	if (m_DepthAttachment)
	{
		m_DepthAttachment->SetActiveTexturePercentage(percentage);
	}

	for (std::shared_ptr<Texture> attachment : m_Attachments)
	{
		attachment->SetActiveTexturePercentage(percentage);
	}
}

float Framebuffer::GetActiveViewportPercentage() const
{
	return m_ActiveViewportPercentage;
}