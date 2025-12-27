#pragma once

#include "Texture.h"

enum class FramebufferSizeAdjustmentMode
{
    ResizeTextureToFramebufferSize,
    ResizeFramebufferToTexutreSize
};

class Framebuffer final
{
    friend class RenderingContext;
public:
    void SetDebugName(ccstr8 name, i32 size);
    void GetDebugName(ccstr8& name, i32& size);

    void AddAttachment(Texture* attachment);

    Texture* GetAttachment(u32 index) const;
    Texture* GetDepthAttachment() const;
    
    void SetAttachment(u32 index, Texture* attachment, FramebufferSizeAdjustmentMode mode);
    void SetDepthAttachment(Texture* attachment, FramebufferSizeAdjustmentMode mode);

	void Copy(Framebuffer* framebuffer, bool colors, bool depth, bool stencil);

    u64 GetNativeHandle() const { return m_NativeHandle; }

	bool Resize(u32 width, u32 height);
	bool Resize(glm::u32vec2 size);

    u32 GetWidth() const { return m_Width; }
    u32 GetHeight() const { return m_Height; }

    ~Framebuffer();

protected:
    Framebuffer(glm::u32vec2 size);
protected:
    uptr m_NativeHandle = 0;

    std::vector<i32> m_AttachmentsNames;
    std::vector<Texture*> m_Attachments;
    
    Texture* m_DepthAttachment;

	u32 m_Width;
	u32 m_Height;
};
