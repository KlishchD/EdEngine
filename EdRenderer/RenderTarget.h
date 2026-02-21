#pragma once

class RenderTarget
{
public:
    RenderTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy sizePolicy, bool enable_uav);
    RenderTarget(ccstr8 name, PixelFormat format, u32 width, u32 height, bool enable_uav);

    ccstr8 GetName() const { return m_Name; }
    const ResourceView& GetView() const { return m_View; }
    const ResourceView& GetSRV() const { return m_SRV; }
    const ResourceView& GetUAV() const { return m_UAV; }
    RenderTargetSizePolicy GetSizePolicy() const { return m_SizePolicy; }

    ~RenderTarget();
protected:
    ccstr8 m_Name;
    ResourceView m_View;
    ResourceView m_SRV;
    ResourceView m_UAV;

    u32 m_Width;
    u32 m_Height;
    RenderTargetSizePolicy m_SizePolicy;
};