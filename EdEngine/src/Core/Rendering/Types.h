#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

enum class WrapMode
{
	Repeat,
	ClampToEdge,
	ClampToBorder,
	MirroredRepeat
};

enum class PixelFormat
{
	RGB8F,
	RGBA8F,
	SRGB8F,
	SRGBA8F,
	RGB16F,
	RGBA16F,
	RGB32F,
	R8F,
	R16F,
	R32F,
	RG8F,
	RG16F,
	RG32F,
	R11G11B10F,

	Depth,
	DepthStencil
};

enum class FilteringMode
{
	Nearest,
	Linear,
	TriLinear
};

enum class BufferUsage
{
	StaticDraw,
	DynamicDraw
};

enum class BlendFactor
{
	One,
	SourceAlpha,
	OneMinusSourceAlpha
};

enum class ShaderDataType 
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
};

enum class FramebufferType 
{
	Framebuffer,
	CubeFramebuffer
};

enum class ShaderType 
{
	None,
	Vertex,
	Geometry,
	Pixel,
	Compute
};

enum class DepthTestFunction
{
	One,
	Greater,
	Lesser
};

enum class Face
{
	Front,
	Back
};

enum class TextureType
{
	Texture2D,
	CubeTexture,
	Texture2DArray
};

enum class BarrierType
{
	AllBits
};

enum class AAMethod
{
	None,
	TAA,
	FXAA
};

enum class RenderTarget
{
	GAlbedo,
	GPosition,
	GNormal,
	GRougnessMetalicEmission,
	GVelocity,
	GDepth,

	SSAO,

	Diffuse,
	Specular,
	Light,

	Bloom,

	AAOutput,
	Resolution
};

enum class DrawMode
{
	Triangles,
	Lines,
	LineStrip
};

class Types
{
public:
	static uint32_t GetChannelNumber(PixelFormat format);
	static uint32_t GetPixelSize(PixelFormat format);
};