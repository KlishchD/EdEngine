#pragma once

#include "Core/Rendering/Framebuffers/Framebuffer.h"

class OpenGLFramebuffer : public Framebuffer
{
public:
	OpenGLFramebuffer(uint32_t width, uint32_t height);

	virtual void CreateAttachment(FramebufferAttachmentType type) override;
	
	virtual void CopyAttachment(const std::shared_ptr<Framebuffer>& framebuffer, int32_t attachment) override;
	virtual void CopyDepthAttachment(const std::shared_ptr<Framebuffer>& framebuffer) override;

	virtual uint32_t GetWidth() const override;
	virtual uint32_t GetHeight() const override;

	virtual void Resize(uint32_t width, uint32_t height) override;

	~OpenGLFramebuffer();
};