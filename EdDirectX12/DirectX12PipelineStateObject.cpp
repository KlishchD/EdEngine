#include "EdDirectX12.h"

#define GetPSO() reinterpret_cast<ID3D12PipelineState*>(m_NativeHandle)

PipelineStateObject::PipelineStateObject(ccstr8 name, const GraphicsPipelineStateObjectBuilder& builder) : m_NativeHandle(0), m_Name(name), m_Type(Graphics), m_VertexStride(0)
{
  Recreate(builder);
  SetDebugName(name);
}

PipelineStateObject::PipelineStateObject(ccstr8 name, const ComputePipelineStateObjectBuilder& bulilder) : m_NativeHandle(0), m_Name(name), m_Type(Compute), m_VertexStride(0)
{
  Recreate(bulilder);
  SetDebugName(name);
}

void PipelineStateObject::Recreate(const GraphicsPipelineStateObjectBuilder& builder)
{
  ED_ASSERT(builder.m_RootSignature, "Root signature must be set.");
  ED_ASSERT(builder.m_ShaderSource != ShaderSource::None, "Shader program must be set.");

  const shader_collection* selected = nullptr;
  shader_collection compiled;

  if (builder.m_ShaderSource == ShaderSource::Path)
  {
    bool status = RenderingContext::Get().CompileShader(builder.m_ShaderDescription, compiled);
    ED_ASSERT(status || m_NativeHandle != 0, "Failed to compile shader and have no substitute.");

    if (!status) return;

    selected = &compiled;
  }
  else
  {
    selected = &builder.m_ShaderCollection;
  }

  D3D12_GRAPHICS_PIPELINE_STATE_DESC description;

  description = { 0 };

  description.pRootSignature = builder.m_RootSignature->GetNativeHandle<ID3D12RootSignature>();

  description.VS.pShaderBytecode = selected->vertex.bytecode;
  description.VS.BytecodeLength = selected->vertex.size;

  description.HS.pShaderBytecode = selected->hull.bytecode;
  description.HS.BytecodeLength = selected->hull.size;

  description.DS.pShaderBytecode = selected->domain.bytecode;
  description.DS.BytecodeLength = selected->domain.size;

  description.GS.pShaderBytecode = selected->geometry.bytecode;
  description.GS.BytecodeLength = selected->geometry.size;

  description.PS.pShaderBytecode = selected->pixel.bytecode;
  description.PS.BytecodeLength = selected->pixel.size;

  description.StreamOutput.pSODeclaration = nullptr;
  description.StreamOutput.NumEntries = 0;
  description.StreamOutput.pBufferStrides = nullptr;
  description.StreamOutput.NumStrides = 0;
  description.StreamOutput.RasterizedStream = 0;

  description.BlendState.AlphaToCoverageEnable = false;
  description.BlendState.IndependentBlendEnable = builder.m_IndependentBlending;

  for (u32 i = 0; i < builder.m_RenderTargets.GetSize(); ++i)
  {
    const GraphicsPipelineStateObjectBuilder::RenderTargetBuilder& source = builder.m_RenderTargets[i];
    D3D12_RENDER_TARGET_BLEND_DESC& destination = description.BlendState.RenderTarget[i];

    destination.BlendEnable = source.m_BlendEnable;
    destination.LogicOpEnable = source.m_LogicOperationEnable;

    destination.SrcBlend = DirectX12Types::ConvertBlendFactor(source.m_SourceColorBlendFactor);
    destination.DestBlend = DirectX12Types::ConvertBlendFactor(source.m_DestinationColorBlendFactor);
    destination.BlendOp = DirectX12Types::ConvertBlendOperation(source.m_ColorBlendOperation);

    destination.SrcBlendAlpha = DirectX12Types::ConvertBlendFactor(source.m_SourceAlphaBlendFactor);
    destination.DestBlendAlpha = DirectX12Types::ConvertBlendFactor(source.m_DestinationAlphaBlendFactor);
    destination.BlendOpAlpha = DirectX12Types::ConvertBlendOperation(source.m_AlphaBlendOperation);

    destination.LogicOp = DirectX12Types::ConvertBlendLogicOperation(source.m_LogicalOperation);

    destination.RenderTargetWriteMask = DirectX12Types::ConvertWriteMask(source.m_WriteMask);
  }

  description.SampleMask = UINT_MAX;

  description.RasterizerState.FillMode = DirectX12Types::ConvertFillMode(builder.m_FillMode);
  description.RasterizerState.CullMode = DirectX12Types::ConvertCullingFace(builder.m_CullFace);
  description.RasterizerState.FrontCounterClockwise = builder.m_FrontFaceWinding == FrontFaceWindingOrder::ClockWise ? false : true;
  description.RasterizerState.DepthBias = builder.m_DepthBias;
  description.RasterizerState.DepthBiasClamp = builder.m_DepthBiasClamp;
  description.RasterizerState.SlopeScaledDepthBias = builder.m_SlopeScaledDepthBias;
  description.RasterizerState.DepthClipEnable = builder.m_DepthClipEnable;
  description.RasterizerState.MultisampleEnable = false;
  description.RasterizerState.AntialiasedLineEnable = false;
  description.RasterizerState.ForcedSampleCount = 0;
  description.RasterizerState.ConservativeRaster = builder.m_ConservativeRasterizationEnabled ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


  description.DepthStencilState.DepthEnable = builder.m_DepthEnabled;
  description.DepthStencilState.DepthWriteMask = builder.m_WriteDepth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
  description.DepthStencilState.DepthFunc = DirectX12Types::ConvertComparisonFunction(builder.m_DepthFunction);
  description.DepthStencilState.StencilEnable = builder.m_StencilEnabled;
  description.DepthStencilState.StencilReadMask = builder.m_StencilMask;
  description.DepthStencilState.StencilWriteMask = builder.m_StencilMask;

  description.DepthStencilState.FrontFace.StencilFailOp = DirectX12Types::ConvertStencilOpeartion(builder.m_FrontFaceStencilResponse.StencilFailOperation);
  description.DepthStencilState.FrontFace.StencilDepthFailOp = DirectX12Types::ConvertStencilOpeartion(builder.m_FrontFaceStencilResponse.StencilAndDepthFailOperation);
  description.DepthStencilState.FrontFace.StencilPassOp = DirectX12Types::ConvertStencilOpeartion(builder.m_FrontFaceStencilResponse.StencilAndDepthPassOperation);
  description.DepthStencilState.FrontFace.StencilFunc = DirectX12Types::ConvertComparisonFunction(builder.m_FrontFaceStencilResponse.StencilComparisonFunction);

  description.DepthStencilState.BackFace.StencilFailOp = DirectX12Types::ConvertStencilOpeartion(builder.m_BackFaceStencilResponse.StencilFailOperation);
  description.DepthStencilState.BackFace.StencilDepthFailOp = DirectX12Types::ConvertStencilOpeartion(builder.m_BackFaceStencilResponse.StencilAndDepthFailOperation);
  description.DepthStencilState.BackFace.StencilPassOp = DirectX12Types::ConvertStencilOpeartion(builder.m_BackFaceStencilResponse.StencilAndDepthPassOperation);
  description.DepthStencilState.BackFace.StencilFunc = DirectX12Types::ConvertComparisonFunction(builder.m_BackFaceStencilResponse.StencilComparisonFunction);

  description.InputLayout.pInputElementDescs = Memory::Get().RequestDynamicMemory<D3D12_INPUT_ELEMENT_DESC>(builder.m_InputLayout.GetSize(), 1, "D3D12InputLayout");

  for (u32 i = 0; i < builder.m_InputLayout.GetSize(); ++i)
  {
    const GraphicsPipelineStateObjectBuilder::InputLayoutElement& source = builder.m_InputLayout[i];
    D3D12_INPUT_ELEMENT_DESC& destination = const_cast<D3D12_INPUT_ELEMENT_DESC&>(description.InputLayout.pInputElementDescs[i]);

    destination.SemanticName = source.Name;
    destination.SemanticIndex = source.Index;
    destination.Format = DirectX12Types::ConvertPixelFormat(source.Format);
    destination.InputSlot = source.Slot;
    destination.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    destination.InputSlotClass = source.PerIsntance ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    destination.InstanceDataStepRate = source.InstanceStepRate;

    m_VertexStride += RenderTypes::GetPixelSize(source.Format);
  }

  description.InputLayout.NumElements = builder.m_InputLayout.GetSize();

  description.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

  description.PrimitiveTopologyType = DirectX12Types::ConvertPrimitiveTopologyType(builder.m_Topology);
  description.NumRenderTargets = builder.m_RenderTargets.GetSize();

  for (u32 i = 0; i < builder.m_RenderTargets.GetSize(); ++i)
  {
    description.RTVFormats[i] = DirectX12Types::ConvertPixelFormat(builder.m_RenderTargets[i].m_Format);
  }

  description.DSVFormat = DirectX12Types::ConvertPixelFormat(builder.m_DepthFormat);

  description.SampleDesc.Count = 1;
  description.SampleDesc.Quality = 0;

  description.NodeMask = 0;

  description.CachedPSO.pCachedBlob = nullptr;
  description.CachedPSO.CachedBlobSizeInBytes = 0;

  description.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

  D3D::Check(g_Device->CreateGraphicsPipelineState(&description, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&m_NativeHandle)));
}

void PipelineStateObject::Recreate(const ComputePipelineStateObjectBuilder& builder)
{
  ED_ASSERT(builder.m_RootSignature, "Root signature must be provided for PSO.");
  ED_ASSERT(builder.m_ShaderSource != ShaderSource::None, "Shader program must be set.");

  shader_collection compiled;

  const shader_collection* selected = nullptr;

  if (builder.m_ShaderSource == ShaderSource::Path)
  {
    bool status = RenderingContext::Get().CompileShader(builder.m_ShaderDescription, compiled);
    ED_ASSERT(status || m_NativeHandle != 0, "Failed to compile shader and have no substitute.");

    if (!status) return;

    selected = &compiled;
  }
  else
  {
    selected = &builder.m_ShaderCollection;
  }

  ED_ASSERT(selected->compute.size, "Compute shader must be provided to compute PSO.");

  D3D12_COMPUTE_PIPELINE_STATE_DESC description{ 0 };

  description.pRootSignature = builder.m_RootSignature->GetNativeHandle<ID3D12RootSignature>();

  description.CS.pShaderBytecode = selected->compute.bytecode;
  description.CS.BytecodeLength = selected->compute.size;

  description.NodeMask = 0;
  description.CachedPSO.pCachedBlob = nullptr;
  description.CachedPSO.CachedBlobSizeInBytes = 0;
  description.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

  D3D::Check(g_Device->CreateComputePipelineState(&description, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&m_NativeHandle)));
}

NAME_METHODS_TEMPLATE(PipelineStateObject, ID3D12PipelineState)

PipelineStateObject::~PipelineStateObject()
{
  GetNativeHandle<ID3D12PipelineState>()->Release();
}
