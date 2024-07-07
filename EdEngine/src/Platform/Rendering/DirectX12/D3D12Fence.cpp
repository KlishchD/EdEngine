#include "D3D12Fence.h"
#include "D3D12RenderingContext.h"

D3D12Fence::D3D12Fence()
{
  gContext->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
  m_Handle = CreateEvent(nullptr, false, false, TEXT("Fence"));
}

void D3D12Fence::Wait(Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue)
{
  uint32_t value = FencesValue;
  queue->Signal(m_Fence.Get(), value);
  ++FencesValue;

  if (m_Fence->GetCompletedValue() < value)
  {
    m_Fence->SetEventOnCompletion(value, m_Handle);
    WaitForSingleObject(m_Handle, INFINITE);
  }
}

