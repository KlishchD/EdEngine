#pragma once

#include <Core/Ed.h>
#include "Core/Rendering/Textures/Texture.h"
#include "Core/Rendering/Textures/Texture2D.h"

enum class FramebufferSizeAdjustmentMode
{
    ResizeTextureToFramebufferSize,
    ResizeFramebufferToTexutreSize
};

class Framebuffer
{
public:
    Framebuffer(uint32_t width, uint32_t height, uint32_t depth);

    virtual void AddAttacment(std::shared_ptr<Texture> attachment) = 0;

    std::shared_ptr<Texture> GetAttachment(int32_t index) const;
    std::shared_ptr<Texture> GetDepthAttachment() const;
    
    virtual void SetAttachment(int32_t index, std::shared_ptr<Texture> attachment, FramebufferSizeAdjustmentMode mode) = 0;
    virtual void SetDepthAttachment(std::shared_ptr<Texture> attachment, FramebufferSizeAdjustmentMode mode) = 0;

	virtual void CopyAttachment(std::shared_ptr<Framebuffer> framebuffer, int32_t attachment) = 0;
	virtual void CopyDepthAttachment(std::shared_ptr<Framebuffer> framebuffer) = 0;

    uint32_t GetID() const;

	virtual void Resize(uint32_t width, uint32_t height, uint32_t depth);
	virtual void Resize(glm::u32vec3 size);

    virtual uint32_t GetWidth() const;
    virtual uint32_t GetHeight() const;
    virtual uint32_t GetDepth() const;

    virtual ~Framebuffer() = default;
protected:
    uint32_t m_Id = 0;

    std::vector<int32_t> m_AttachmentsNames;
    std::vector<std::shared_ptr<Texture>> m_Attachments;
    
    std::shared_ptr<Texture> m_DepthAttachment;

	uint32_t m_Width;
	uint32_t m_Height;
    uint32_t m_Depth;
};
