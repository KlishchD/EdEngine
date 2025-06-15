#pragma once

enum class WrapMode : u8
{
	Wrap = 0,
	Clamp,
	Border,
    Mirror,
    MirrorOnce,
    Count
};

enum class PixelFormat : u8
{
    R8F,
    R16F,
    R32F,
    
    RG8F,
    RG16F,
    RG32F,

    RGB8F,
    RGB16F,
    RGB32F,

    RGBA8F,
    RGBA16F,
    RGBA32F,

    SRGBA8F,
	R11G11B10F,

	Depth,
	DepthStencil
};

enum class PrimitiveFillingMode
{
    Wireframe,
    Solid
};

enum class StencilOperation
{
    Keep,
    Zero,
    Replace,
    IncrementAndClamp,
    DecrementAndClamp,
    Invert,
    IncrementAndWrap,
    DecrementAndWrap
};

enum class ComparisonFunction
{
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

enum class RenderTargetWriteMask : u8
{
    None = 0,
    R    = (1 << 0),
    G    = (1 << 1),
    B    = (1 << 2),
    A    = (1 << 3),
    RG   = R | G,
    RB   = R | B,
    RA   = R | A,
    GB   = G | B,
    GA   = G | A,
    BA   = B | A,
    RGB  = R | G | B,
    GBA  = G | B | A,
    RGBA = R | G | B | A
};

enum class BlendOperation
{
    Add,
    Subtract,
    SubtractReversed,
    Min,
    Max
};

enum class BlendFactor
{
    Zero,
    One,
    SourceColor,
    DestinationColor,
    InrtedSourceColor,
    InvertedDestinationColor,
    SourceAlpha,
    InvertedSourceAlpha,
    DestinationAlpha,
    InvertedDestinationAlpha,
    SourceAlphaClamped,
    CustomBlendFactor, // Requires rendering context extensions
    InvertedCustomBlendFactor
};

enum class BlendLogicalOperation
{
    None,
    Zero,
    One,
    CopySource,
    CopyInvertedSource,
    InvertDestination,
    And,
    InvertedAnd,
    Or,
    InvertedOr,
    Xor,
    InvertedXor,
    SourceAndInvertedDestination,
    InvertedSourceAndDestination,
    SourceOrInvertedDestination,
    InvertedSourceOrDestination
};

enum class PrimitiveTopology
{
    PointsList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
    LineListAdjacency,
    LineStripAdjacency,
    TriangleListAdjacency,
    TriangleStripAdjacency,
};

enum class PrimitiveCullingMode
{
    None,
    Front,
    Back
};

enum class FrontFaceWindingOrder
{
    ClockWise,
    CounterClockWise
};

enum class FilteringMode : u8
{
	Point,
	Bilinear,
	Trilinear,
    Aniso
};

enum class BufferUsage : u8
{
	StaticDraw,
	DynamicDraw
};

enum class ShaderDataType : u8
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
};

enum ShaderType : u8
{
	ST_None = 0,
	ST_Vertex = (1 << 1),
	ST_Geometry = (1 << 2),
	ST_Pixel = (1 << 3),
    ST_Domain = (1 << 4),
    ST_Hull = (1 << 5),
	ST_Compute = (1 << 6),
    ST_Count = 7
};

enum class DepthTestFunction : u8
{
	One,
	Greater,
	Lesser
};

enum class Face : u8
{
	Front,
	Back
};

enum class BarrierType : u8
{
	AllBits
};

enum class DrawMode : u8
{
	Triangles,
	Lines,
	LineStrip
};

enum class BufferType : u8
{
    Vertex,
    Index,
    Uniform
};

enum class TextureType : u8 
{
    Texture2D,
    TextureArray
};

enum class ResourceState : u32
{
    Common = (1 << 0),
    VertexAndConstantBuffer = (1 << 1),
    IndexBuffer = (1 << 2),
    RenderTarget = (1 << 3),
    UnorderedAccess = (1 << 4),
    DepthWrite = (1 << 5),
    DepthRead = (1 << 6),
    NonPixelShaderResource = (1 << 7),
    PixelShaderResource = (1 << 8),
    StreamOut = (1 << 9),
    InderectArgument = (1 << 10),
    CopyDestination = (1 << 11),
    CopySource = (1 << 12),
    ResolveDestination = (1 << 13),
    ResolveSource = (1 << 14),
    RayTracingAccelerationStructure = (1 << 15),
    ShadingRateSource = (1 << 16),
    Present = (1 << 19),
    Predication = (1 << 20),
    VideoDecodeRead = (1 << 21),
    VideoDecodeWrite = (1 << 22),
    VideoProcessRead = (1 << 23),
    VideoProcessWrite = (1 << 24),
    VideoEncodeRead = (1 << 25),
    VideoEncodeWrite = (1 << 26),
    GenericRead = (1 << 27),

    ShaderRead = PixelShaderResource | NonPixelShaderResource,
};

enum class CommandListType : u8
{
    Direct,
    Compute,
    Copy,
    Bundle,

    CoreCount = 3
};

enum class DescriptorHeapType : u8
{
    CBV,
    SRV,
    UAV,
    Sampler,
    RTV,
    DSV,
    Count
};

enum class HeapType : u8
{
    Default,
    Upload,
    Readback
};

enum ResourceFlags : u16
{
    RF_None = 0,
    RF_AllowRenderTarget = (1 << 0),
    RF_AllowDepthStencil = (1 << 1),
    RF_AllowUnorderedAccess = (1 << 2),
    RF_DenyShaderResource = (1 << 3),
    RF_AllowCrossAdapter = (1 << 4),
    RF_AllowSimultaneousAccess = (1 << 5),
    RF_VideoDecodeReferenceOnly = (1 << 6),
    RF_VideoEncodeReferenceOnly = (1 << 7),
    RF_RaytracingAccelerationStructure = (1 << 8),
    RF_HolderResource = (1 << 9) // Resource is not an owner of its NativeHandle, so it will no release it.
};

enum class DescriptorRangeType
{
    SRV,
    UAV,
    CBV,
};

enum ShaderVisbilityFlags
{
    SVF_Vertex = (1 << 0),
    SVF_Hull = (1 << 1),
    SVF_Domain = (1 << 2),
    SVF_Geometry = (1 << 3),
    SVF_Pixel = (1 << 4),
    SVF_All = SVF_Vertex | SVF_Hull | SVF_Domain | SVF_Geometry | SVF_Pixel
};

enum RootSignatureFlags : u32
{
    RSF_None = 0,
    RSF_AllowInputAssembler = (1 << 0),
    RSF_DenyVertexShaderRootAccess = (1 << 1),
    RSF_DenyHullShaderRootAccess = (1 << 2),
    RSF_DenyDomainShaderRootAccess = (1 << 3),
    RSF_DenyGeometryShaderRootAccess = (1 << 4),
    RSF_DenyPixelShaderRootAccess = (1 << 5),
    RSF_AllowStreamOutput = (1 << 6),
    RSF_LocalRootSignature = (1 << 7),
    RSF_DenyAmplificationShaderRootAccess = (1 << 8),
    RSF_DenyMeshShaderRootAccess = (1 << 9),
    RSF_CBV_SRV_UAV_HEAP_DirectlyIndexed = (1 << 10),
    RSF_SamplerHeapDirectlyIndexed = (1 << 11)
};

enum HeapFlags : u32
{
    HF_None = (1 << 0),
    HF_Shared = (1 << 1),
    HF_DenyBuffers = (1 << 2),
    HF_AllowDisplay = (1 << 3),
    HF_SharedCrossAdapter = (1 << 4),
    HF_DenyRTAndDSTextures = (1 << 5),
    HF_DenyNonRTAndDSTextures = (1 << 6),
    HF_HardwareProtected = (1 << 7),
    HF_AllowWriteWatch = (1 << 8),
    HF_AllowShaderAtomics = (1 << 9),
    HF_CreateNotResident = (1 << 10),
    HF_CreateNotZeroed = (1 << 11),
    HF_ToolsUseManualWriteTracking = (1 << 12),
    HF_AllowAllBuffersAndTextures = (1 << 13),
    HF_AllowOnlyBuffers = (1 << 14),
    HF_AllowOnlyNonRTAndDSTextures = (1 << 15),
    HF_AllowOnlyRTAndDSTextures = (1 << 16)
};

enum class RenderTargetSizePolicy
{
    Upscale16,
    Upscale8,
    Upscale4,
    Upscale2,
    Full,
    Downscale2,
    Downscale4,
    Downscale8,
    Downscale16,
    Custom
};

class RenderTypes
{
public:
	static u32 GetChannelNumber(PixelFormat format);
	static u32 GetPixelSize(PixelFormat format);

    static u32 ConvertShaderDataTypeCount(ShaderDataType type);
    static u32 ConvertShaderDataTypeSize(ShaderDataType type);

    static ccstr16 ConvertShaderEntrypoint(ShaderType type);
    static ccstr16 ConvertShaderTarget(ShaderType type);

    static f32 ConvertRenderTargetSizePolicy(RenderTargetSizePolicy policy);

    static PixelFormat CompressType(PixelFormat format, u32 channals);
};