#pragma once

class DirectX12Types
{
public:
  static ccstr8 ConvertFeatureLevelToString(D3D_FEATURE_LEVEL level);
  static uint32_t ConvertFormatDataSize(DXGI_FORMAT format);

  static D3D12_COMMAND_LIST_TYPE ConvertCommandListType(CommandListType type);
  static DXGI_FORMAT ConvertPixelFormat(PixelFormat format);
  static D3D12_PRIMITIVE_TOPOLOGY ConverPrimitiveTopology(PrimitiveTopology topology);
  static D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopologyType(PrimitiveTopology topology);

  static bool ContainsResourceState(ResourceState stateA, ResourceState stateB);
  static D3D12_RESOURCE_STATES ConvertResourceState(ResourceState state);

  static D3D12_DESCRIPTOR_HEAP_TYPE ConvertDescriptorHeapType(DescriptorHeapType type);
  static D3D12_HEAP_FLAGS ConvertHeapFlags(HeapFlags flags);

  static D3D12_BLEND ConvertBlendFactor(BlendFactor factor);
  static D3D12_BLEND_OP ConvertBlendOperation(BlendOperation operation);
  static D3D12_LOGIC_OP ConvertBlendLogicOperation(BlendLogicalOperation operation);
  static u8 ConvertWriteMask(RenderTargetWriteMask mask);

  static D3D12_FILL_MODE ConvertFillMode(PrimitiveFillingMode mode);
  static D3D12_CULL_MODE ConvertCullingFace(PrimitiveCullingMode face);

  static D3D12_COMPARISON_FUNC ConvertComparisonFunction(ComparisonFunction function);
  static D3D12_STENCIL_OP ConvertStencilOpeartion(StencilOperation operation);

  static D3D12_HEAP_TYPE ConvertHeapType(HeapType type);
  static D3D12_CPU_PAGE_PROPERTY ConvertCPUPageProperty(HeapType type);
  static D3D12_MEMORY_POOL ConvertMemoryPool(HeapType type);

  static D3D12_RESOURCE_FLAGS ConvertResoruceFlags(ResourceFlags flags);

  static D3D12_DESCRIPTOR_RANGE_TYPE ConvertRangeType(DescriptorRangeType type);

  static D3D12_FILTER ConvertFilteringMode(FilteringMode mode);
  static D3D12_TEXTURE_ADDRESS_MODE ConvertWrapMode(WrapMode mode);

  static D3D12_ROOT_SIGNATURE_FLAGS ConvertRootSignatureFlags(RootSignatureFlags flags);

  static D3D12_ROOT_PARAMETER_TYPE ConvertRooParmaterType(DescriptorHeapType type);
  static D3D12_SHADER_VISIBILITY ConvertShaderVisibility(ShaderVisbilityFlags visibility);
};
