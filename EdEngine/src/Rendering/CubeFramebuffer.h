#pragma once

#include "BaseFramebuffer.h"

class CubeFramebuffer: public BaseFramebuffer
{
public:
    CubeFramebuffer(int32_t size);

    virtual void CreateAttachment(FramebufferAttachmentType type) override;

    void AttachFace(int32_t face) const;
    void AttachLayers() const;

    void Resize(int32_t size);

    int32_t GetSize() const;
    virtual int32_t GetWidth() const override;
    virtual int32_t GetHeight() const override;
protected:
    int32_t m_Size;
};
