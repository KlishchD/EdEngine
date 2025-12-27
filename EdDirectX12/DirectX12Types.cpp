#include "EdDirectX12.h"
#include "DirectX12Types.h"

ccstr8 DirectX12Types::ConvertFeatureLevelToString(D3D_FEATURE_LEVEL level)
{
  switch (level)
  {
  case D3D_FEATURE_LEVEL_1_0_CORE: return "D3D_FEATURE_LEVEL_1_0_CORE";
  case D3D_FEATURE_LEVEL_9_1: return "D3D_FEATURE_LEVEL_9_1";
  case D3D_FEATURE_LEVEL_9_2: return "D3D_FEATURE_LEVEL_9_2";
  case D3D_FEATURE_LEVEL_9_3: return "D3D_FEATURE_LEVEL_9_3";
  case D3D_FEATURE_LEVEL_10_0: return "D3D_FEATURE_LEVEL_10_0";
  case D3D_FEATURE_LEVEL_10_1: return "D3D_FEATURE_LEVEL_10_1";
  case D3D_FEATURE_LEVEL_11_0: return "D3D_FEATURE_LEVEL_11_0";
  case D3D_FEATURE_LEVEL_11_1: return "D3D_FEATURE_LEVEL_11_1";
  case D3D_FEATURE_LEVEL_12_0: return "D3D_FEATURE_LEVEL_12_0";
  case D3D_FEATURE_LEVEL_12_1: return "D3D_FEATURE_LEVEL_12_1";
  case D3D_FEATURE_LEVEL_12_2: return "D3D_FEATURE_LEVEL_12_2";
  default:
    ED_ASSERT(0, "There is no such feature level");
  }

  return "";
}

uint32_t DirectX12Types::ConvertFormatDataSize(DXGI_FORMAT format)
{
  switch (format)
  {
  case DXGI_FORMAT_R8G8B8A8_UNORM: return 4 * sizeof(uint8_t);
  case DXGI_FORMAT_R32_FLOAT: return sizeof(uint32_t);
  default:
    ED_ASSERT(0, "Format is not supported");
  }

  return 0;
}

D3D12_COMMAND_LIST_TYPE DirectX12Types::ConvertCommandListType(CommandListType type)
{
	switch (type)
	{
	case CommandListType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case CommandListType::Bundle: return D3D12_COMMAND_LIST_TYPE_BUNDLE;
	case CommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	case CommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
	default:
		ED_ASSERT(0, "Command list type is not supported.");
	}

	return D3D12_COMMAND_LIST_TYPE_COPY;
}

DXGI_FORMAT DirectX12Types::ConvertPixelFormat(PixelFormat format)
{
	switch (format)
	{
	case PixelFormat::R8F:          return DXGI_FORMAT_R8_UNORM;
	case PixelFormat::R16F:         return DXGI_FORMAT_R16_FLOAT;
	case PixelFormat::R32F:         return DXGI_FORMAT_R32_FLOAT;

	case PixelFormat::RG8F:         return DXGI_FORMAT_R8G8_UNORM;
	case PixelFormat::RG16F:        return DXGI_FORMAT_R16G16_FLOAT;
	case PixelFormat::RG32F:        return DXGI_FORMAT_R32G32_FLOAT;
	
	case PixelFormat::RGB8F:        ED_ASSERT(0, "RGB8F is not supported"); return DXGI_FORMAT_UNKNOWN;
	case PixelFormat::RGB16F:       ED_ASSERT(0, "RGB16F is not supported"); return DXGI_FORMAT_UNKNOWN;
	case PixelFormat::RGB32F:       return DXGI_FORMAT_R32G32B32_FLOAT;
	
	case PixelFormat::RGBA8F:       return DXGI_FORMAT_R8G8B8A8_UNORM;
	case PixelFormat::RGBA16F:      return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case PixelFormat::RGBA32F:      return DXGI_FORMAT_R32G32B32A32_FLOAT;
	
	case PixelFormat::SRGBA8F:      return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	
	case PixelFormat::R11G11B10F:   return DXGI_FORMAT_R11G11B10_FLOAT;
	
	case PixelFormat::Depth:        return DXGI_FORMAT_D32_FLOAT;
	case PixelFormat::DepthStencil: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	
	default:
        ED_ASSERT(0, "Pixel format is not supported.");
	}

	return DXGI_FORMAT_UNKNOWN;
}

D3D12_PRIMITIVE_TOPOLOGY DirectX12Types::ConverPrimitiveTopology(PrimitiveTopology topology)
{
	switch (topology)
	{
	case PrimitiveTopology::PointsList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case PrimitiveTopology::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case PrimitiveTopology::LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case PrimitiveTopology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case PrimitiveTopology::TriangleFan: ED_ASSERT(0, "D3D12 doesn't support TriangleFan topology."); return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	case PrimitiveTopology::LineListAdjacency: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case PrimitiveTopology::LineStripAdjacency: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case PrimitiveTopology::TriangleListAdjacency: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case PrimitiveTopology::TriangleStripAdjacency: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	default:
		ED_ASSERT(0, "Primitive topology is not supported.");
		break;
	}

	return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE DirectX12Types::ConvertPrimitiveTopologyType(PrimitiveTopology topology)
{
    switch (topology)
    {
    case PrimitiveTopology::PointsList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case PrimitiveTopology::LineList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case PrimitiveTopology::LineStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case PrimitiveTopology::TriangleList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case PrimitiveTopology::TriangleStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case PrimitiveTopology::TriangleFan: ED_ASSERT(0, "D3D12 doesn't support TriangleFan topology."); return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    case PrimitiveTopology::LineListAdjacency: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case PrimitiveTopology::LineStripAdjacency: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case PrimitiveTopology::TriangleListAdjacency: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case PrimitiveTopology::TriangleStripAdjacency: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    default:
        ED_ASSERT_CONTEXT(D3D12API, 0, "Primitive topology is not supported.");
    }

    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
}

bool DirectX12Types::ContainsResourceState(ResourceState stateA, ResourceState stateB)
{
	u32 convertedA = static_cast<u32>(stateA);
	u32 convertedB = static_cast<u32>(stateB);
    return (convertedA & convertedB) == convertedB;
}

D3D12_RESOURCE_STATES DirectX12Types::ConvertResourceState(ResourceState state)
{
	u32 result = 0;

	if (ContainsResourceState(state, ResourceState::Common))                          result |= D3D12_RESOURCE_STATE_COMMON;
	if (ContainsResourceState(state, ResourceState::VertexAndConstantBuffer))         result |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	if (ContainsResourceState(state, ResourceState::IndexBuffer))                     result |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
	if (ContainsResourceState(state, ResourceState::RenderTarget))                    result |= D3D12_RESOURCE_STATE_RENDER_TARGET;
	if (ContainsResourceState(state, ResourceState::UnorderedAccess))                 result |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	if (ContainsResourceState(state, ResourceState::DepthWrite))                      result |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
	if (ContainsResourceState(state, ResourceState::DepthRead))                       result |= D3D12_RESOURCE_STATE_DEPTH_READ;
	if (ContainsResourceState(state, ResourceState::NonPixelShaderResource))          result |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	if (ContainsResourceState(state, ResourceState::PixelShaderResource))             result |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	if (ContainsResourceState(state, ResourceState::StreamOut))                       result |= D3D12_RESOURCE_STATE_STREAM_OUT;
	if (ContainsResourceState(state, ResourceState::InderectArgument))                result |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
	if (ContainsResourceState(state, ResourceState::CopyDestination))                 result |= D3D12_RESOURCE_STATE_COPY_DEST;
	if (ContainsResourceState(state, ResourceState::CopySource))                      result |= D3D12_RESOURCE_STATE_COPY_SOURCE;
	if (ContainsResourceState(state, ResourceState::ResolveDestination))              result |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
	if (ContainsResourceState(state, ResourceState::ResolveSource))                   result |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	if (ContainsResourceState(state, ResourceState::RayTracingAccelerationStructure)) result |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	if (ContainsResourceState(state, ResourceState::ShadingRateSource))               result |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
	if (ContainsResourceState(state, ResourceState::Present))                         result |= D3D12_RESOURCE_STATE_PRESENT;
	if (ContainsResourceState(state, ResourceState::Predication))                     result |= D3D12_RESOURCE_STATE_PREDICATION;
	if (ContainsResourceState(state, ResourceState::VideoDecodeRead))                 result |= D3D12_RESOURCE_STATE_VIDEO_DECODE_READ;
	if (ContainsResourceState(state, ResourceState::VideoDecodeWrite))                result |= D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE;
	if (ContainsResourceState(state, ResourceState::VideoProcessRead))                result |= D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ;
	if (ContainsResourceState(state, ResourceState::VideoProcessWrite))               result |= D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE;
	if (ContainsResourceState(state, ResourceState::VideoEncodeRead))                 result |= D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ;
	if (ContainsResourceState(state, ResourceState::VideoEncodeWrite))                result |= D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;
	if (ContainsResourceState(state, ResourceState::GenericRead))                     result |= D3D12_RESOURCE_STATE_GENERIC_READ;

	return static_cast<D3D12_RESOURCE_STATES>(result);
}

D3D12_DESCRIPTOR_HEAP_TYPE DirectX12Types::ConvertDescriptorHeapType(DescriptorHeapType type)
{
	switch (type)
	{
	case DescriptorHeapType::CBV: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	case DescriptorHeapType::SRV: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	case DescriptorHeapType::UAV: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	case DescriptorHeapType::Sampler: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	case DescriptorHeapType::RTV: return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	case DescriptorHeapType::DSV: return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	case DescriptorHeapType::Count: ED_ASSERT(0, "D3D12 doesn't support Count descriptor heap type."); return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
	default:
		ED_ASSERT(0, "Descriptor heap type is not supported.");
		break;
	}

	return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
}

D3D12_HEAP_FLAGS DirectX12Types::ConvertHeapFlags(HeapFlags flags)
{
	u32 result = 0;

	if (flags & HF_Shared) result |= D3D12_HEAP_FLAG_SHARED;
	if (flags & HF_DenyBuffers) result |= D3D12_HEAP_FLAG_DENY_BUFFERS;
	if (flags & HF_AllowDisplay) result |= D3D12_HEAP_FLAG_ALLOW_DISPLAY;
	if (flags & HF_SharedCrossAdapter) result |= D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER;
	if (flags & HF_DenyRTAndDSTextures) result |= D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	if (flags & HF_DenyNonRTAndDSTextures) result |= D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	if (flags & HF_HardwareProtected) result |= D3D12_HEAP_FLAG_HARDWARE_PROTECTED;
	if (flags & HF_AllowWriteWatch) result |= D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH;
	if (flags & HF_AllowShaderAtomics) result |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
	if (flags & HF_CreateNotResident) result |= D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT;
	if (flags & HF_CreateNotZeroed) result |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
	if (flags & HF_ToolsUseManualWriteTracking) result |= D3D12_HEAP_FLAG_TOOLS_USE_MANUAL_WRITE_TRACKING;
	if (flags & HF_AllowAllBuffersAndTextures) result |= D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
	if (flags & HF_AllowOnlyBuffers) result |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
	if (flags & HF_AllowOnlyNonRTAndDSTextures) result |= D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
	if (flags & HF_AllowOnlyRTAndDSTextures) result |= D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;

	return static_cast<D3D12_HEAP_FLAGS>(result);
}

D3D12_BLEND DirectX12Types::ConvertBlendFactor(BlendFactor factor)
{
	switch (factor)
	{
	case BlendFactor::Zero: return D3D12_BLEND_ZERO;
	case BlendFactor::One: return D3D12_BLEND_ONE;
	case BlendFactor::SourceColor: return D3D12_BLEND_SRC_COLOR;
	case BlendFactor::DestinationColor: return D3D12_BLEND_DEST_COLOR;
	case BlendFactor::InrtedSourceColor: return D3D12_BLEND_INV_SRC_COLOR;
	case BlendFactor::InvertedDestinationColor: return D3D12_BLEND_INV_DEST_COLOR;
	case BlendFactor::SourceAlpha: return D3D12_BLEND_SRC_ALPHA;
	case BlendFactor::InvertedSourceAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
	case BlendFactor::DestinationAlpha: return D3D12_BLEND_DEST_ALPHA;
	case BlendFactor::InvertedDestinationAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
	case BlendFactor::SourceAlphaClamped: return D3D12_BLEND_SRC_ALPHA_SAT;
	case BlendFactor::CustomBlendFactor: return D3D12_BLEND_BLEND_FACTOR;
	case BlendFactor::InvertedCustomBlendFactor: return D3D12_BLEND_INV_BLEND_FACTOR;
	default:
		ED_ASSERT(0, "Blend factor is not supported.");
		break;
	}

	return D3D12_BLEND_ZERO;
}

D3D12_BLEND_OP DirectX12Types::ConvertBlendOperation(BlendOperation operation)
{
	switch (operation)
	{
	case BlendOperation::Add: return D3D12_BLEND_OP_ADD;
	case BlendOperation::Subtract: return D3D12_BLEND_OP_SUBTRACT;
	case BlendOperation::SubtractReversed: return D3D12_BLEND_OP_REV_SUBTRACT;
	case BlendOperation::Min: return D3D12_BLEND_OP_MIN;
	case BlendOperation::Max: return D3D12_BLEND_OP_MAX;
	default:
		ED_ASSERT(0, "Blend operation is not supported.");
		break;
	}

	return D3D12_BLEND_OP_SUBTRACT;
}

D3D12_LOGIC_OP DirectX12Types::ConvertBlendLogicOperation(BlendLogicalOperation operation)
{
	switch (operation)
	{
	case BlendLogicalOperation::None: return D3D12_LOGIC_OP_NOOP;
	case BlendLogicalOperation::Zero: return D3D12_LOGIC_OP_CLEAR;
	case BlendLogicalOperation::One: return D3D12_LOGIC_OP_SET;
	case BlendLogicalOperation::CopySource: return D3D12_LOGIC_OP_COPY;
	case BlendLogicalOperation::CopyInvertedSource: return D3D12_LOGIC_OP_COPY_INVERTED;
	case BlendLogicalOperation::InvertDestination: return D3D12_LOGIC_OP_INVERT;
	case BlendLogicalOperation::And: return D3D12_LOGIC_OP_AND;
	case BlendLogicalOperation::InvertedAnd: return D3D12_LOGIC_OP_NAND;
	case BlendLogicalOperation::Or: return D3D12_LOGIC_OP_OR;
	case BlendLogicalOperation::InvertedOr: return D3D12_LOGIC_OP_NOR;
	case BlendLogicalOperation::Xor: return D3D12_LOGIC_OP_XOR;
	case BlendLogicalOperation::InvertedXor: return D3D12_LOGIC_OP_EQUIV;
	case BlendLogicalOperation::SourceAndInvertedDestination: return D3D12_LOGIC_OP_AND_REVERSE;
	case BlendLogicalOperation::InvertedSourceAndDestination: return D3D12_LOGIC_OP_AND_INVERTED;
	case BlendLogicalOperation::SourceOrInvertedDestination: return D3D12_LOGIC_OP_OR_REVERSE;
	case BlendLogicalOperation::InvertedSourceOrDestination: return D3D12_LOGIC_OP_OR_INVERTED;
	default:
		ED_ASSERT(0, "Blend logic operator is not supported.");
		break;
	}

	return D3D12_LOGIC_OP_CLEAR;
}

u8 DirectX12Types::ConvertWriteMask(RenderTargetWriteMask mask)
{
	u8 result = 0;
	u8 value = static_cast<u8>(mask);

	if (value & static_cast<u8>(RenderTargetWriteMask::R)) result |= D3D12_COLOR_WRITE_ENABLE_RED;
	if (value & static_cast<u8>(RenderTargetWriteMask::G)) result |= D3D12_COLOR_WRITE_ENABLE_GREEN;
	if (value & static_cast<u8>(RenderTargetWriteMask::B)) result |= D3D12_COLOR_WRITE_ENABLE_BLUE;
	if (value & static_cast<u8>(RenderTargetWriteMask::A)) result |= D3D12_COLOR_WRITE_ENABLE_ALPHA;

	return result;
}

D3D12_FILL_MODE DirectX12Types::ConvertFillMode(PrimitiveFillingMode mode)
{
    switch (mode)
    {
    case PrimitiveFillingMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
    case PrimitiveFillingMode::Solid: return D3D12_FILL_MODE_SOLID;
    default:
        ED_ASSERT(0, "Fill mode is not supported.");
    }

    return D3D12_FILL_MODE_SOLID;
}

D3D12_CULL_MODE DirectX12Types::ConvertCullingFace(PrimitiveCullingMode face)
{
    switch (face)
    {
    case PrimitiveCullingMode::None: return D3D12_CULL_MODE_NONE;
    case PrimitiveCullingMode::Front: return D3D12_CULL_MODE_FRONT;
    case PrimitiveCullingMode::Back: return D3D12_CULL_MODE_BACK;
    default:
        ED_ASSERT(0, "Culling face is not supported.");
    }

    return D3D12_CULL_MODE_NONE;
}

D3D12_COMPARISON_FUNC DirectX12Types::ConvertComparisonFunction(ComparisonFunction function)
{
    switch (function)
    {
    case ComparisonFunction::Never:        return D3D12_COMPARISON_FUNC_NEVER;
    case ComparisonFunction::Less:         return D3D12_COMPARISON_FUNC_LESS;
    case ComparisonFunction::Equal:        return D3D12_COMPARISON_FUNC_EQUAL;
    case ComparisonFunction::LessEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case ComparisonFunction::Greater:      return D3D12_COMPARISON_FUNC_GREATER;
    case ComparisonFunction::NotEqual:     return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case ComparisonFunction::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case ComparisonFunction::Always:       return D3D12_COMPARISON_FUNC_ALWAYS;
    default:
        ED_ASSERT(0, "Comparison function is not supported.");
    }

    return D3D12_COMPARISON_FUNC_NEVER;
}

D3D12_STENCIL_OP DirectX12Types::ConvertStencilOpeartion(StencilOperation operation)
{
    switch (operation)
    {
    case StencilOperation::Keep:              return D3D12_STENCIL_OP_KEEP;
    case StencilOperation::Zero:              return D3D12_STENCIL_OP_ZERO;
    case StencilOperation::Replace:           return D3D12_STENCIL_OP_REPLACE;
    case StencilOperation::IncrementAndClamp: return D3D12_STENCIL_OP_INCR_SAT;
    case StencilOperation::DecrementAndClamp: return D3D12_STENCIL_OP_DECR_SAT;
    case StencilOperation::Invert:            return D3D12_STENCIL_OP_INVERT;
    case StencilOperation::IncrementAndWrap:  return D3D12_STENCIL_OP_INCR;
    case StencilOperation::DecrementAndWrap:  return D3D12_STENCIL_OP_DECR;
    default:
        ED_ASSERT(0, "Stencil operation is not supported.");
    }

    return D3D12_STENCIL_OP_ZERO;
}

D3D12_HEAP_TYPE DirectX12Types::ConvertHeapType(HeapType type)
{
	switch (type)
	{
	case HeapType::Default: return D3D12_HEAP_TYPE_DEFAULT;
	case HeapType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
	case HeapType::Readback: return D3D12_HEAP_TYPE_READBACK;
	default:
		ED_ASSERT(0, "Heap type is not supported.");
		break;
	}

	return D3D12_HEAP_TYPE_CUSTOM;
}

D3D12_CPU_PAGE_PROPERTY DirectX12Types::ConvertCPUPageProperty(HeapType type)
{
	switch (type)
	{
	case HeapType::Default: return D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
	case HeapType::Upload: return D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	case HeapType::Readback: return D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	default:
        ED_ASSERT(0, "CPU page property is not supported.");
        break;
	}

	return D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
}

D3D12_MEMORY_POOL DirectX12Types::ConvertMemoryPool(HeapType type)
{
	switch (type)
	{
	case HeapType::Default: return D3D12_MEMORY_POOL_L1;
	case HeapType::Upload: return D3D12_MEMORY_POOL_L0;
	case HeapType::Readback: return D3D12_MEMORY_POOL_L0;
	default:
        ED_ASSERT(0, "Mempory pool is not supported.");
        break;
	}

	return D3D12_MEMORY_POOL_UNKNOWN;
}

D3D12_RESOURCE_FLAGS DirectX12Types::ConvertResoruceFlags(ResourceFlags flags)
{
	D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;

	if (flags & RF_AllowRenderTarget) result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (flags & RF_AllowDepthStencil) result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	if (flags & RF_AllowUnorderedAccess) result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (flags & RF_DenyShaderResource) result |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
	if (flags & RF_AllowCrossAdapter) result |= D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
	if (flags & RF_AllowSimultaneousAccess) result |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
	if (flags & RF_VideoDecodeReferenceOnly) result |= D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY;
	if (flags & RF_VideoEncodeReferenceOnly) result |= D3D12_RESOURCE_FLAG_VIDEO_ENCODE_REFERENCE_ONLY;
	if (flags & RF_RaytracingAccelerationStructure) result |= D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;

	return result;
}

D3D12_DESCRIPTOR_RANGE_TYPE DirectX12Types::ConvertRangeType(DescriptorRangeType type)
{
	switch (type)
	{
	case DescriptorRangeType::SRV: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	case DescriptorRangeType::UAV: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	case DescriptorRangeType::CBV: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	default:
		ED_ASSERT(0, "Descriptor range type is not supported.");
		break;
	}

	return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
}

D3D12_FILTER DirectX12Types::ConvertFilteringMode(FilteringMode mode)
{
	switch (mode)
	{
	case FilteringMode::Point: return D3D12_FILTER_MIN_MAG_MIP_POINT;
	case FilteringMode::Bilinear: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	case FilteringMode::Trilinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	case FilteringMode::Aniso: return D3D12_FILTER_ANISOTROPIC;
	default:
        ED_ASSERT(0, "Filtering mode is not supported.");
        break;
	}

	return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
}

D3D12_TEXTURE_ADDRESS_MODE DirectX12Types::ConvertWrapMode(WrapMode mode)
{
	switch (mode)
	{
	case WrapMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case WrapMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case WrapMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	case WrapMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case WrapMode::MirrorOnce: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	default:
		ED_ASSERT(0, "Wrap mode is not supported");
		break;
	}

	return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
}

D3D12_ROOT_SIGNATURE_FLAGS DirectX12Types::ConvertRootSignatureFlags(RootSignatureFlags flags)
{
	D3D12_ROOT_SIGNATURE_FLAGS result = D3D12_ROOT_SIGNATURE_FLAG_NONE;

    if (flags & RSF_AllowInputAssembler) result |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    if (flags & RSF_DenyVertexShaderRootAccess) result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
    if (flags & RSF_DenyHullShaderRootAccess) result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
    if (flags & RSF_DenyDomainShaderRootAccess) result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
    if (flags & RSF_DenyGeometryShaderRootAccess) result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
    if (flags & RSF_DenyPixelShaderRootAccess) result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
    if (flags & RSF_AllowStreamOutput) result |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
	if (flags & RSF_LocalRootSignature) result |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
	if (flags & RSF_DenyAmplificationShaderRootAccess) result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
	if (flags & RSF_DenyMeshShaderRootAccess) result |= D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
	if (flags & RSF_CBV_SRV_UAV_HEAP_DirectlyIndexed) result |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
	if (flags & RSF_SamplerHeapDirectlyIndexed) result |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

	return result;
}

D3D12_ROOT_PARAMETER_TYPE DirectX12Types::ConvertRooParmaterType(DescriptorHeapType type)
{
	switch (type)
	{
	case DescriptorHeapType::CBV: return D3D12_ROOT_PARAMETER_TYPE_CBV;
	case DescriptorHeapType::SRV: return D3D12_ROOT_PARAMETER_TYPE_SRV;
	case DescriptorHeapType::UAV: return D3D12_ROOT_PARAMETER_TYPE_UAV;
	case DescriptorHeapType::Sampler:
	case DescriptorHeapType::RTV:
	case DescriptorHeapType::DSV:
	case DescriptorHeapType::Count:
	default:
		ED_ASSERT(0, "Provided root parameter type is not supported.");
		break;
	}
	
	return D3D12_ROOT_PARAMETER_TYPE_UAV;
}

D3D12_SHADER_VISIBILITY DirectX12Types::ConvertShaderVisibility(ShaderVisbilityFlags visibility)
{
	switch (visibility)
	{
	case SVF_Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
	case SVF_Hull: return D3D12_SHADER_VISIBILITY_HULL;
	case SVF_Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
	case SVF_Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
	case SVF_Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
	case SVF_All: return D3D12_SHADER_VISIBILITY_ALL;
	default:
        ED_ASSERT(0, "Provided shader visibility is not supported.");
        break;
	}

	return D3D12_SHADER_VISIBILITY_ALL;
}
