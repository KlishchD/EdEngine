#include "D3D12Helper.h"

D3D12_RESOURCE_BARRIER D3D12Helper::TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource1> resource, uint32_t subresoruce, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
  D3D12_RESOURCE_BARRIER barrier{};
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  barrier.Transition.pResource = resource.Get();
  barrier.Transition.Subresource = subresoruce;
  barrier.Transition.StateBefore = before;
  barrier.Transition.StateAfter = after;
  return barrier;
}

D3D12_RESOURCE_BARRIER D3D12Helper::TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource1> resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
  D3D12_RESOURCE_BARRIER barrier{};
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  barrier.Transition.pResource = resource.Get();
  barrier.Transition.Subresource = 0;
  barrier.Transition.StateBefore = before;
  barrier.Transition.StateAfter = after;
  return barrier;
}

uint32_t D3D12Helper::CalculateResourceSize(const D3D12_RESOURCE_DESC& description)
{
  return description.Width * description.Height * description.DepthOrArraySize * D3D12Types::ConvertFormatDataSize(description.Format);
}
