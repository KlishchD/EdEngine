#pragma once

#include "Core/Rendering/Types.h"
#include <cstdint>

class OpenGLTypes {
public:
	static uint32_t ConvertWrapMode(WrapMode mode);
	static uint32_t ConvertFilteringMode(FilteringMode mode, bool bMipMapEnabled = false);
	static uint32_t ConvertPixelFormat(PixelFormat format);
	static uint32_t ConvertPixelExternalFormat(PixelFormat format);
	static uint32_t ConvertDataType(PixelFormat format);
	static uint32_t ConvertBufferUsage(BufferUsage usage);

	static uint32_t ConvertShaderDataTypeCount(ShaderDataType type);
	static uint32_t ConvertShaderDataTypeSize(ShaderDataType type);
	static uint32_t ConvertShaderDataTypeType(ShaderDataType type);

	static uint32_t ConvertBlendFactor(BlendFactor factor);

	static uint32_t ConvertShaderType(ShaderType type);

	static uint32_t ConvertDepthTestFunction(DepthTestFunction function);
	
	static uint32_t ConvertFace(Face face);

	static uint32_t ConverTextureType(TextureType type);

	static uint32_t ConvertBarrierType(BarrierType type);

	static uint32_t ConvertDrawMode(DrawMode mode);
};