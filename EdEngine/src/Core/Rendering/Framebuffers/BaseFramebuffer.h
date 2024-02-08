#pragma once

#include <Core/Ed.h>
#include "Core/Rendering/Textures/Texture.h"
#include "Core/Rendering/Textures/Texture2D.h"

enum class FramebufferAttachmentType
{
    Color,
    Color16,
    Depth,
    DepthStencil,
    Position,
    Direction,
    Velocity,
    Distance,
    Bloom
};

class BaseFramebuffer
{
public:
    virtual void CreateAttachment(FramebufferAttachmentType type) = 0;

    std::shared_ptr<Texture> GetAttachment(int32_t index) const;
    std::shared_ptr<Texture> GetDepthAttachment() const;
    std::shared_ptr<Texture> GetDepthStencilAttachment() const;
    
    virtual void SetAttachment(int32_t index, std::shared_ptr<Texture> texture, bool bResizeFramebufferToTextureSize = false) = 0;

    uint32_t GetID() const;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual ~BaseFramebuffer() = default;
protected:
    uint32_t m_Id = 0;

    std::vector<int32_t> m_AttachmentsNames;
    std::vector<std::shared_ptr<Texture>> m_Attachments;
    
    std::shared_ptr<Texture> m_DepthAttachment;
    std::shared_ptr<Texture> m_DepthStencilAttachment;
};
