#pragma once

#include "Core/Rendering/Framebuffers/CubeFramebuffer.h"

class OpenGLCubeFramebuffer : public CubeFramebuffer
{
public:
	OpenGLCubeFramebuffer(int32_t size);

	virtual void CreateAttachment(FramebufferAttachmentType type) override;

	virtual void AttachFace(int32_t face) override;
	virtual void AttachLayers() override;

	virtual uint32_t GetWidth() const override;
	virtual uint32_t GetHeight() const override;

	virtual void Resize(uint32_t size) override;

	virtual ~OpenGLCubeFramebuffer() override;
};