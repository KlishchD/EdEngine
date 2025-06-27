#include "EdRendererPrivate.h"
#include "UAVTarget.h"

UAVTarget::UAVTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy policy, u32 mips) : m_Name(name), m_SizePolicy(policy), m_Width(0), m_Height(0)
{
    RenderingContext& context = RenderingContext::Get();
    m_Views = context.CreateUAVTarget(policy, mips, format, name);
    m_SRV = context.GetSRVHeap()->CreateView(DescriptorHeapType::SRV, m_Views[0].Viewed);
}

UAVTarget::UAVTarget(ccstr8 name, PixelFormat format, u32 width, u32 height, u32 mips) : m_Name(name), m_SizePolicy(RenderTargetSizePolicy::Custom), m_Width(width), m_Height(height)
{
    RenderingContext& context = RenderingContext::Get();
    
    m_Views = context.CreateUAVTarget(width, height, mips, format, name);
    m_SRV = context.GetSRVHeap()->CreateView(DescriptorHeapType::SRV, m_Views[0].Viewed);
}

UAVTarget::~UAVTarget()
{
    RenderingContext& context = RenderingContext::Get();
    context.FreeUAVTargets(m_Views);
    context.GetSRVHeap()->FreeView(m_SRV);
}
