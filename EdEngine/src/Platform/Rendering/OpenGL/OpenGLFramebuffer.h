#pragma once

#include "Core/Rendering/Framebuffer.h"

class OpenGLFramebuffer : public Framebuffer
{
public:
	OpenGLFramebuffer(const FramebufferSpecification& specification);

	virtual void AddAttachment(std::shared_ptr<Texture> attachment);
	
	virtual void SetAttachment(int32_t index, std::shared_ptr<Texture> attachment, FramebufferSizeAdjustmentMode mode) override;
	virtual void SetDepthAttachment(std::shared_ptr<Texture> attachment, FramebufferSizeAdjustmentMode mode) override;

	virtual void CopyAttachment(std::shared_ptr<Framebuffer> framebuffer, int32_t attachment) override;
	virtual void CopyDepthAttachment(std::shared_ptr<Framebuffer> framebuffer) override;

	virtual uint32_t GetWidth() const override;
	virtual uint32_t GetHeight() const override;

	~OpenGLFramebuffer();
};