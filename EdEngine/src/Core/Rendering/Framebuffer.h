#pragma once

#include "Core/Ed.h"
#include "Core/Rendering/Textures/Texture.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Resource.h"

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

struct RenderTargetSpecification : public ResourceSpecification
{
    FramebufferAttachmentType Type;
};

struct FramebufferSpecification : public ResourceSpecification
{
    glm::u32vec3 Size = glm::u32vec3(1);
    TextureType TextureType = TextureType::Texture2D;
    std::vector<RenderTargetSpecification> RenderTargets;
};

enum class FramebufferSizeAdjustmentMode
{
    ResizeTextureToFramebufferSize,
    ResizeFramebufferToTexutreSize
};

class Framebuffer : public Resource
{
public:
    Framebuffer(const FramebufferSpecification& specification);

    virtual void AddAttachment(std::shared_ptr<Texture> attachment) = 0;

    template<typename T>
    std::shared_ptr<T> GetAttachment(int32_t index) const
    {
        return std::static_pointer_cast<T>(GetAttachment(index));
    }

    template<typename T>
    std::shared_ptr<T> GetDepthAttachment() const
    {
        return std::static_pointer_cast<T>(GetDepthAttachment());
    }

    std::shared_ptr<Texture> GetAttachment(int32_t index) const;
    std::shared_ptr<Texture> GetDepthAttachment() const;
    
    virtual void SetAttachment(int32_t index, std::shared_ptr<Texture> attachment, FramebufferSizeAdjustmentMode mode) = 0;
    virtual void SetDepthAttachment(std::shared_ptr<Texture> attachment, FramebufferSizeAdjustmentMode mode) = 0;

	virtual void CopyAttachment(std::shared_ptr<Framebuffer> framebuffer, int32_t attachment) = 0;
	virtual void CopyDepthAttachment(std::shared_ptr<Framebuffer> framebuffer) = 0;

    uint32_t GetID() const;

	virtual bool Resize(uint32_t width, uint32_t height, uint32_t depth);
	virtual bool Resize(glm::u32vec3 size);

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
