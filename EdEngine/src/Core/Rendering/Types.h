#pragma once

enum class WrapMode
{
	Repeat,
	ClampToEdge,
	ClampToBorder
};

enum class PixelFormat
{
	RGB8F,
	RGBA8F,
	SRGB8F,
	SRGBA8F,
	RGB16F,
	RGB32F,
	R8F,
	R16F,
	R32F,

	Depth,
	DepthStencil
};

enum class FilteringMode
{
	Nearest,
	Linear
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
	CubeTexture
};