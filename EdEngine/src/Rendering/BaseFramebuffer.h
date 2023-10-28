#pragma once

#include <Core/Ed.h>
#include "Core/Assets/Texture.h"

enum class FramebufferAttachmentType
{
    Color,
    Depth,
    DepthStencil,
    Position,
    Direction,
    Distance
};

class BaseFramebuffer
{
public:
    virtual ~BaseFramebuffer() = default;
    BaseFramebuffer();

    virtual void CreateAttachment(FramebufferAttachmentType type) = 0;
    void CopyAttachment(const std::shared_ptr<BaseFramebuffer>& framebuffer, int32_t attachment);
    void CopyDepthAttachment(const std::shared_ptr<BaseFramebuffer>& framebuffer);

    std::shared_ptr<Texture> GetAttachment(int32_t index) const;
    std::shared_ptr<Texture> GetDepthAttachment() const;
    
    void Bind();
    void Unbind();

    int32_t GetId() const;

    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
protected:
    uint32_t m_Id;

    std::vector<int32_t> m_AttachmentsNames;
    std::vector<std::shared_ptr<Texture>> m_Attachments;
    
    std::shared_ptr<Texture> m_DepthAttachment;
    std::shared_ptr<Texture> m_DepthStencilAttachment;
};
