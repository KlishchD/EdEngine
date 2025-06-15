#include "EdRendererPrivate.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy policy) : m_Name(name), m_SizePolicy(policy), m_Width(0), m_Height(0)
{
    RenderingContext& context = RenderingContext::Get();
    m_View = context.CreateRenderTarget(policy, format, name);
    m_SRV = context.GetSRVHeap()->CreateView(DescriptorHeapType::SRV, m_View.Viewed);
}

RenderTarget::RenderTarget(ccstr8 name, PixelFormat format, u32 width, u32 height) : m_Name(name), m_SizePolicy(RenderTargetSizePolicy::Custom), m_Width(width), m_Height(height)
{
    RenderingContext& context = RenderingContext::Get();
    m_View = context.CreateRenderTarget(width, height, format, name, m_View);
    m_SRV = context.GetSRVHeap()->CreateView(DescriptorHeapType::SRV, m_View.Viewed);
}

RenderTarget::~RenderTarget()
{
    RenderingContext& context = RenderingContext::Get();
    context.FreeRenderTarget(m_View);
    context.GetSRVHeap()->FreeView(m_SRV);
}
