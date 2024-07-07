#pragma once

#include "Platform/Rendering/DirectX12/EdD3D12Rendering.h"

class D3D12Helper
{
public:
  static D3D12_RESOURCE_BARRIER TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource1> resource, uint32_t subresoruce, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
  static D3D12_RESOURCE_BARRIER TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource1> resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
};
