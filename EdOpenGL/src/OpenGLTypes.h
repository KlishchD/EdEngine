#pragma once

class OpenGLTypes {
public:
	static u32 ConvertWrapMode(WrapMode mode);
	static u32 ConvertFilteringMode(FilteringMode mode, bool bMipMapEnabled = false);
	static u32 ConvertPixelFormat(PixelFormat format);
	static u32 ConvertPixelExternalFormat(PixelFormat format);
	static u32 ConvertDataType(PixelFormat format);
	static u32 ConvertBufferUsage(BufferUsage usage);

	static u32 ConvertShaderDataTypeType(ShaderDataType type);

	static u32 ConvertBlendFactor(BlendFactor factor);

	static u32 ConvertShaderType(ShaderType type);

	static u32 ConvertDepthTestFunction(DepthTestFunction function);
	
	static u32 ConvertFace(Face face);

	static u32 ConvertTextureType(TextureType type);

	static u32 ConvertBarrierType(BarrierType type);

	static u32 ConvertDrawMode(DrawMode mode);
};