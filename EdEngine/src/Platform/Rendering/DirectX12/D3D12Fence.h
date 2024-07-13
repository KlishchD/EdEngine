#pragma once

#include "EdD3D12Rendering.h"

class D3D12RenderingContext;

class D3D12Fence
{
public:
  D3D12Fence();

  void Wait(Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue);

protected:
  Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
  HANDLE m_Handle;

  static inline uint32_t FencesValue = 1;
};
