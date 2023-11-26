#pragma once

#include <cstdint>
#include <memory>

#include <Core/Ed.h>

#include "BaseFramebuffer.h"

class Framebuffer: public BaseFramebuffer
{
public:
    Framebuffer(uint32_t width, uint32_t height);

    virtual void Resize(uint32_t width, uint32_t height) override;
    
    virtual int32_t GetWidth() const override;
    virtual int32_t GetHeight() const override;
    
    virtual void CreateAttachment(FramebufferAttachmentType type) override;
private:
    uint32_t m_Width;
    uint32_t m_Height;
};
