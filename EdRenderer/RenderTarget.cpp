#include "EdRendererPrivate.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy policy, bool enable_uav) : m_Name(name), m_SizePolicy(policy), m_Width(0), m_Height(0)
{
  RenderingContext& context = RenderingContext::Get();
  m_View = context.CreateRenderTarget(policy, format, name, enable_uav);

  m_SRV = context.GetSRVHeap()->CreateView(DescriptorHeapType::SRV, m_View.Viewed);

  if (!formats::is_depth_format(format) && enable_uav)
  {
    m_UAV = context.GetUAVHeap()->CreateView(DescriptorHeapType::UAV, m_View.Viewed);
  }
}

RenderTarget::RenderTarget(ccstr8 name, PixelFormat format, u32 width, u32 height, bool enable_uav) : m_Name(name), m_SizePolicy(RenderTargetSizePolicy::Custom), m_Width(width), m_Height(height)
{
  RenderingContext& context = RenderingContext::Get();
  m_View = context.CreateRenderTarget(width, height, format, name, enable_uav);

  m_SRV = context.GetSRVHeap()->CreateView(DescriptorHeapType::SRV, m_View.Viewed);

  if (!formats::is_depth_format(format) && enable_uav)
  {
    m_UAV = context.GetUAVHeap()->CreateView(DescriptorHeapType::UAV, m_View.Viewed);
  }
}

RenderTarget::~RenderTarget()
{
  RenderingContext& context = RenderingContext::Get();
  context.FreeRenderTarget(m_View);
  context.GetSRVHeap()->FreeView(m_SRV);
  context.GetUAVHeap()->FreeView(m_UAV);
}
