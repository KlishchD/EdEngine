#include "EdDirectX12.h"

#define GetPSO() reinterpret_cast<ID3D12PipelineState*>(m_NativeHandle)

D3D12_SHADER_BYTECODE ExtractShaderByteCode(ShaderType type, ShaderProgram* program)
{
    D3D12_SHADER_BYTECODE result;
    if (Shader* shader = program->GetShader(type))
    {
        result.pShaderBytecode = shader->GetBytecode();
        result.BytecodeLength = shader->GetBytecodeLength();
    }
    else
    {
        result.pShaderBytecode = 0;
        result.BytecodeLength = 0;
    }

    return result;
}

PipelineStateObject::PipelineStateObject(ccstr8 name, const GraphicsPipelineStateObjectBuilder& builder) : m_Name(name), m_Type(Graphics), m_VertexStride(0)
{
    ED_ASSERT(builder.m_RootSignature, "Root signature must be set.");
    ED_ASSERT(builder.m_Program, "Shader program must be set.");

    if (builder.m_Program->NeedsRecompilation())
    {
        builder.m_Program->Compile(DEBUG_BUILD || DEVELOPMENT_BUILD, nullptr);
    }

    #if ALLOW_PSO_RECREATION == 1
         m_Description = PSODescriptionsAllocator().Allocate(sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
         m_Program = builder.m_Program;
         D3D12_GRAPHICS_PIPELINE_STATE_DESC& description = *reinterpret_cast<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>(m_Description);
    #else
        D3D12_GRAPHICS_PIPELINE_STATE_DESC description;
    #endif

    description = { 0 };

    description.pRootSignature = builder.m_RootSignature->GetNativeHandle<ID3D12RootSignature>();
    description.VS = ExtractShaderByteCode(ST_Vertex, builder.m_Program);
    description.PS = ExtractShaderByteCode(ST_Pixel, builder.m_Program);
    description.DS = ExtractShaderByteCode(ST_Domain, builder.m_Program);
    description.HS = ExtractShaderByteCode(ST_Hull, builder.m_Program);
    description.GS = ExtractShaderByteCode(ST_Geometry, builder.m_Program);

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

    SetDebugName(name);
}

PipelineStateObject::PipelineStateObject(ccstr8 name, RootSignature* signature, ShaderProgram* program) : m_Name(name), m_Type(Compute)
{
    ED_ASSERT(signature, "Root signature must be set.");
    ED_ASSERT(program, "Shader program must be set.");

    if (program->NeedsRecompilation())
    {
        program->Compile(DEBUG_BUILD || DEVELOPMENT_BUILD, nullptr);
    }

#if ALLOW_PSO_RECREATION == 1
    m_Description = PSODescriptionsAllocator().Allocate(sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
    m_Program = program;
    D3D12_COMPUTE_PIPELINE_STATE_DESC& description = *reinterpret_cast<D3D12_COMPUTE_PIPELINE_STATE_DESC*>(m_Description);
#else
    D3D12_COMPUTE_PIPELINE_STATE_DESC description;
#endif
    
    description = { 0 };

    description.pRootSignature = signature->GetNativeHandle<ID3D12RootSignature>();
    description.CS = ExtractShaderByteCode(ST_Compute, program);
    description.NodeMask = 0;
    description.CachedPSO.pCachedBlob = nullptr;
    description.CachedPSO.CachedBlobSizeInBytes = 0;
    description.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    D3D::Check(g_Device->CreateComputePipelineState(&description, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&m_NativeHandle)));

    SetDebugName(name);
}

NAME_METHODS_TEMPLATE(PipelineStateObject, ID3D12PipelineState)

void PipelineStateObject::Recreate()
{
#if ALLOW_PSO_RECREATION == 1
    GetPSO()->Release();

    if (m_Program->HasShader(ST_Compute))
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC& description = *reinterpret_cast<D3D12_COMPUTE_PIPELINE_STATE_DESC*>(m_Description);
        description.CS = ExtractShaderByteCode(ST_Compute, m_Program);
        D3D::Check(g_Device->CreateComputePipelineState(&description, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&m_NativeHandle)));
    }
    else
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC& description = *reinterpret_cast<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>(m_Description);
        description.VS = ExtractShaderByteCode(ST_Vertex, m_Program);
        description.PS = ExtractShaderByteCode(ST_Pixel, m_Program);
        description.DS = ExtractShaderByteCode(ST_Domain, m_Program);
        description.HS = ExtractShaderByteCode(ST_Hull, m_Program);
        description.GS = ExtractShaderByteCode(ST_Geometry, m_Program);

        D3D::Check(g_Device->CreateGraphicsPipelineState(&description, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&m_NativeHandle)));
    }
#endif
}

PipelineStateObject::~PipelineStateObject()
{
    GetNativeHandle<ID3D12PipelineState>()->Release();

#if ALLOW_PSO_RECREATION == 1
    u32 size = m_Program->HasShader(ST_Compute) ? sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC) : sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC);
    PSODescriptionsAllocator().Deallocate(m_Description, size);
#endif
}
