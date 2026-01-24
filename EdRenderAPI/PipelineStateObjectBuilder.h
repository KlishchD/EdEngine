#pragma once

enum class ShaderSource
{
  None,
  Collection,
  Path
};

template <typename ChildType>
class PipelineStateObjectBuilder
{
public:
  friend class PipelineStateObject;
  friend class RenderingContext;

  __forceinline ChildType& SetRootSignature(RootSignature* signature)
  {
    m_RootSignature = signature; 
    return *reinterpret_cast<ChildType*>(this);
  }

  __forceinline ChildType& SetShader(const shader_collection& collection)
  {
    ED_ASSERT(m_ShaderSource == ShaderSource::None, "Can not change shader source.");
    m_ShaderCollection = collection;
    m_ShaderSource = ShaderSource::Collection;
    return *reinterpret_cast<ChildType*>(this);
  }

  __forceinline ChildType& SetShader(shader_collection&& collection)
  {
    ED_ASSERT(m_ShaderSource == ShaderSource::None, "Can not change shader source.");
    m_ShaderCollection = std::move(collection);
    m_ShaderSource = ShaderSource::Collection;
    return *reinterpret_cast<ChildType*>(this);
  }

  __forceinline ChildType& SetShader(const ShaderPath& path)
  {
    ED_ASSERT(m_ShaderSource == ShaderSource::None, "Can not change shader source.");
    m_ShaderDescription.source = path;
    m_ShaderSource = ShaderSource::Path;
    return *reinterpret_cast<ChildType*>(this);
  }

  template <typename value_type>
  __forceinline ChildType& AddShaderDefine(ccstr8 name, const value_type& value)
  {
    ED_ASSERT(m_ShaderSource == ShaderSource::Path, "Shader defines must be provided after shader path is set.");
    m_ShaderDescription.append_define(name, value);
    return *reinterpret_cast<ChildType*>(this);
  }
protected:
  RootSignature* m_RootSignature = nullptr;

  shader_collection m_ShaderCollection = {};
  shader_description m_ShaderDescription = {};
  ShaderSource m_ShaderSource = ShaderSource::None;
};

class GraphicsPipelineStateObjectBuilder : public PipelineStateObjectBuilder<GraphicsPipelineStateObjectBuilder>
{
  struct StencilResponse
  {
    StencilOperation StencilFailOperation = StencilOperation::Keep;
    StencilOperation StencilAndDepthFailOperation = StencilOperation::Keep;
    StencilOperation StencilAndDepthPassOperation = StencilOperation::Keep;
    ComparisonFunction StencilComparisonFunction = ComparisonFunction::Never;
  };

  struct InputLayoutElement
  {
    ccstr8 Name;
    u32 Index;
    u32 Slot;
    PixelFormat Format;
    bool PerIsntance;
    u32 InstanceStepRate;
  };

  class RenderTargetBuilder
  {
    friend class PipelineStateObject;
  public:
    __forceinline RenderTargetBuilder& SetFormat(PixelFormat format) { m_Format = format; return *this; }
    __forceinline RenderTargetBuilder& SetBlend(bool blend) { m_BlendEnable = blend; return *this; }

    __forceinline RenderTargetBuilder& SetColorBlend(BlendFactor soruce, BlendFactor destination, BlendOperation operation)
    {
      m_BlendEnable = true;
      m_SourceColorBlendFactor = soruce;
      m_DestinationColorBlendFactor = destination;
      m_ColorBlendOperation = operation;
      return *this;
    }

    __forceinline RenderTargetBuilder& SetAlphaBlend(BlendFactor source, BlendFactor destination, BlendOperation operation)
    {
      m_BlendEnable = true;
      m_SourceAlphaBlendFactor = source;
      m_DestinationAlphaBlendFactor = destination;
      m_AlphaBlendOperation = operation;
      return *this;
    }

    __forceinline RenderTargetBuilder& SetLogicalOperation(BlendLogicalOperation operation)
    {
      m_LogicOperationEnable = true;
      m_LogicalOperation = operation;
      return *this;
    }

    __forceinline RenderTargetBuilder& SetWriteMask(RenderTargetWriteMask mask) { m_WriteMask = mask; return *this; }
  protected:
    PixelFormat m_Format = PixelFormat::RGBA8F;

    bool m_BlendEnable = false;
    bool m_LogicOperationEnable = false;

    BlendFactor m_SourceColorBlendFactor = BlendFactor::SourceAlpha;
    BlendFactor m_DestinationColorBlendFactor = BlendFactor::InvertedSourceAlpha;
    BlendOperation m_ColorBlendOperation = BlendOperation::Add;

    BlendFactor m_SourceAlphaBlendFactor = BlendFactor::One;
    BlendFactor m_DestinationAlphaBlendFactor = BlendFactor::Zero;
    BlendOperation m_AlphaBlendOperation = BlendOperation::Add;

    BlendLogicalOperation m_LogicalOperation = BlendLogicalOperation::And;

    RenderTargetWriteMask m_WriteMask = RenderTargetWriteMask::RGBA;
  };
public:
  friend class PipelineStateObject;

  static constexpr u32 MaxRenderTargetsCount = 8;
  GraphicsPipelineStateObjectBuilder() : m_RenderTargets(MaxRenderTargetsCount) 
  { }

  __forceinline GraphicsPipelineStateObjectBuilder& SetIndependentBlending(bool enabled) { m_IndependentBlending = enabled; return *this; }

  __forceinline GraphicsPipelineStateObjectBuilder& SetFillMode(PrimitiveFillingMode mode) { m_FillMode = mode; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetCullFace(PrimitiveCullingMode mode) { m_CullFace = mode; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetWinding(FrontFaceWindingOrder winding) { m_FrontFaceWinding = winding; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetDepthBias(i32 bias) { m_DepthBias = bias; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetSlopeScaledDepthBias(float bias) { m_SlopeScaledDepthBias = bias; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetDepthClip(bool enabled) { m_DepthClipEnable = enabled; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetConservativeRasterization(bool enabled) { m_ConservativeRasterizationEnabled = enabled; return *this; }

  __forceinline RenderTargetBuilder& AddRenderTarget() { return m_RenderTargets.Add(); }

  __forceinline GraphicsPipelineStateObjectBuilder& SetDepthStencil(bool depth, bool stencil) { m_DepthEnabled = depth; m_StencilEnabled = stencil; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetWriteDepthStencil(bool depth, bool stencil) { m_WriteDepth = depth; m_WriteStencil = stencil; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetStencilMask(u8 mask) { m_StencilMask = mask; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetDepthFunction(ComparisonFunction function) { m_DepthFunction = function; return *this; }
  __forceinline GraphicsPipelineStateObjectBuilder& SetFrontFaceStencilRespones(StencilOperation stencilFail, StencilOperation depthStencilFail, StencilOperation depthStencilPass, ComparisonFunction function)
  {
    m_FrontFaceStencilResponse.StencilFailOperation = stencilFail;
    m_FrontFaceStencilResponse.StencilAndDepthFailOperation = depthStencilFail;
    m_FrontFaceStencilResponse.StencilAndDepthPassOperation = depthStencilPass;
    m_FrontFaceStencilResponse.StencilComparisonFunction = function;
    return *this;
  }

  __forceinline GraphicsPipelineStateObjectBuilder& SetBackFaceStencilRespones(StencilOperation stencilFail, StencilOperation depthStencilFail, StencilOperation depthStencilPass, ComparisonFunction function)
  {
    m_BackFaceStencilResponse.StencilFailOperation = stencilFail;
    m_BackFaceStencilResponse.StencilAndDepthFailOperation = depthStencilFail;
    m_BackFaceStencilResponse.StencilAndDepthPassOperation = depthStencilPass;
    m_BackFaceStencilResponse.StencilComparisonFunction = function;
    return *this;
  }

  __forceinline GraphicsPipelineStateObjectBuilder& SetDepthFormat(PixelFormat format) { m_DepthFormat = format; return *this; }

  __forceinline GraphicsPipelineStateObjectBuilder& AddInputElement(ccstr8 name, u32 index, PixelFormat format)
  {
    InputLayoutElement& element = m_InputLayout.Add();
    element.Name = name;
    element.Index = index;
    element.Format = format;
    element.PerIsntance = false;
    element.InstanceStepRate = 0;
    return *this;
  }

  __forceinline GraphicsPipelineStateObjectBuilder& AddInputElement(ccstr8 name, u32 index, PixelFormat format, u32 slot, u32 instanceStepRate)
  {
    InputLayoutElement& element = m_InputLayout.Add();
    element.Name = name;
    element.Index = index;
    element.Slot = slot;
    element.Format = format;
    element.PerIsntance = true;
    element.InstanceStepRate = instanceStepRate;
    return *this;
  }

  __forceinline GraphicsPipelineStateObjectBuilder& SetTopology(PrimitiveTopology topology) { m_Topology = topology; return *this; }
protected:
  bool m_IndependentBlending = false;

  PrimitiveFillingMode m_FillMode = PrimitiveFillingMode::Solid;
  PrimitiveCullingMode m_CullFace = PrimitiveCullingMode::Back;
  FrontFaceWindingOrder m_FrontFaceWinding = FrontFaceWindingOrder::ClockWise;

  i32 m_DepthBias = 0;
  float m_DepthBiasClamp = 0;
  float m_SlopeScaledDepthBias = 0;

  bool m_DepthClipEnable = true;
  bool m_ConservativeRasterizationEnabled = false;

  Array<RenderTargetBuilder> m_RenderTargets;

  bool m_DepthEnabled = false;
  bool m_StencilEnabled = false;
  bool m_WriteDepth = true;
  bool m_WriteStencil = true;
  u8 m_StencilMask = 0;
  ComparisonFunction m_DepthFunction = ComparisonFunction::Less;
  StencilResponse m_FrontFaceStencilResponse;
  StencilResponse m_BackFaceStencilResponse;
  PixelFormat m_DepthFormat = PixelFormat::Depth;

  Array<InputLayoutElement> m_InputLayout;

  PrimitiveTopology m_Topology = PrimitiveTopology::TriangleList;
};

class ComputePipelineStateObjectBuilder : public PipelineStateObjectBuilder<ComputePipelineStateObjectBuilder>
{
public:
  friend class PipelineStateObject;

  ComputePipelineStateObjectBuilder()
  { }
};