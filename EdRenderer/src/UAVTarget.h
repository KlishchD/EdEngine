#pragma once

// TODO:
// This system of RenderTargets and UAVTargets is really shitty
// Too many interdependencies which makes it not as intuitive 
// to debug, should be able to refactor it quite soon.
// And now I have use cases for such a logic, so there it
// should be easier to improve it.
class UAVTarget
{
public:
    UAVTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy sizePolicy, u32 mips);
    UAVTarget(ccstr8 name, PixelFormat format, u32 width, u32 height, u32 mips);

    ccstr8 GetName() const { return m_Name; }
    Resource* GetResource() const { return m_Views[0].Viewed; }
    const ResourceView& GetView(u32 mip = 0) const { return m_Views[mip]; }
    const ResourceView& GetSRV() const { return m_SRV; }
    RenderTargetSizePolicy GetSizePolicy() const { return m_SizePolicy; }

    ~UAVTarget();
protected:
    ccstr8 m_Name;
    Array<ResourceView> m_Views;
    ResourceView m_SRV;

    u32 m_Width;
    u32 m_Height;
    RenderTargetSizePolicy m_SizePolicy;
};