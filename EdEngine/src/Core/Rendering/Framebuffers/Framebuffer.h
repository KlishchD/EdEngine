#pragma once

#include "BaseFramebuffer.h"

class Framebuffer: public BaseFramebuffer
{
public:
    Framebuffer(uint32_t width, uint32_t height);

	virtual void CopyAttachment(std::shared_ptr<Framebuffer> framebuffer, int32_t attachment) = 0;
	virtual void CopyDepthAttachment(std::shared_ptr<Framebuffer> framebuffer) = 0;

    virtual void Resize(uint32_t width, uint32_t height) = 0;
    
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
protected:
    uint32_t m_Width;
    uint32_t m_Height;
};
