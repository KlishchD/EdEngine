#include "OpenGLTypes.h"
#include "Core/Macros.h"
#include "Core/Rendering/EdRendering.h"

uint32_t OpenGLTypes::ConvertWrapMode(WrapMode mode)
{
	switch (mode)
	{
	case WrapMode::Repeat:           return GL_REPEAT;
	case WrapMode::ClampToBorder:    return GL_CLAMP_TO_BORDER;
	case WrapMode::ClampToEdge:      return GL_CLAMP_TO_EDGE;
	case WrapMode::MirroredRepeat:   return GL_MIRRORED_REPEAT;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Wrap mode is not supported");
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertFilteringMode(FilteringMode mode, bool bMipMapEnbaled)
{
	if (bMipMapEnbaled)
	{
		switch (mode)
		{
		case FilteringMode::Nearest:    return GL_NEAREST_MIPMAP_LINEAR;
		case FilteringMode::Linear:     return GL_LINEAR_MIPMAP_LINEAR;
		case FilteringMode::TriLinear:  return GL_LINEAR_MIPMAP_LINEAR;
		}
	}
	else
	{
		switch (mode)
		{
		case FilteringMode::Nearest:    return GL_NEAREST;
		case FilteringMode::Linear:     return GL_LINEAR;
		case FilteringMode::TriLinear:  return GL_LINEAR_MIPMAP_LINEAR;
		}
	}

	ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Filtering mode is not supported");
	return 0;

}

uint32_t OpenGLTypes::ConvertPixelFormat(PixelFormat format)
{
	switch (format)
	{
	case PixelFormat::RGB8F:          return GL_RGB8;
	case PixelFormat::RGBA8F:         return GL_RGBA8;
	case PixelFormat::SRGB8F:         return GL_SRGB8;
	case PixelFormat::SRGBA8F:        return GL_SRGB8_ALPHA8;
	case PixelFormat::RGB16F:         return GL_RGB16F;
	case PixelFormat::RGBA16F:        return GL_RGBA16F;
	case PixelFormat::RGB32F:         return GL_RGB32F;
	case PixelFormat::R8F:            return GL_R8;
	case PixelFormat::R16F:           return GL_R16F;
	case PixelFormat::R32F:           return GL_R32F;
	case PixelFormat::RG8F:           return GL_RG8;
	case PixelFormat::RG16F:          return GL_RG16F;
	case PixelFormat::RG32F:          return GL_RG32F;
	case PixelFormat::R11G11B10F:     return GL_R11F_G11F_B10F;
	case PixelFormat::Depth:          return GL_DEPTH_COMPONENT;
	case PixelFormat::DepthStencil:   return GL_DEPTH24_STENCIL8;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Pixel type is not supported");
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertPixelExternalFormat(PixelFormat format)
{
	switch (format)
	{
	case PixelFormat::RGB8F:          return GL_RGB;
	case PixelFormat::RGBA8F:         return GL_RGBA;
	case PixelFormat::SRGB8F:         return GL_RGB;
	case PixelFormat::SRGBA8F:        return GL_RGBA;
	case PixelFormat::RGB16F:         return GL_RGB;
	case PixelFormat::RGBA16F:        return GL_RGBA;
	case PixelFormat::RGB32F:         return GL_RGB;
	case PixelFormat::R8F:            return GL_RED;
	case PixelFormat::R16F:           return GL_RED;
	case PixelFormat::R32F:           return GL_RED;
	case PixelFormat::RG8F:           return GL_RG;
	case PixelFormat::RG16F:          return GL_RG;
	case PixelFormat::RG32F:          return GL_RG;
	case PixelFormat::R11G11B10F:     return GL_RGB;
	case PixelFormat::Depth:          return GL_DEPTH_COMPONENT;
	case PixelFormat::DepthStencil:   return GL_DEPTH_STENCIL;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Pixel type is not supported");
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertDataType(PixelFormat format)
{
	switch (format)
	{
	case PixelFormat::RGB8F:          return GL_UNSIGNED_BYTE;
	case PixelFormat::RGBA8F:         return GL_UNSIGNED_BYTE;
	case PixelFormat::SRGB8F:         return GL_UNSIGNED_BYTE;
	case PixelFormat::SRGBA8F:        return GL_UNSIGNED_BYTE;
	case PixelFormat::RGB16F:         return GL_FLOAT;
	case PixelFormat::RGBA16F:        return GL_FLOAT;
	case PixelFormat::RGB32F:         return GL_FLOAT;
	case PixelFormat::R8F:            return GL_UNSIGNED_BYTE;
	case PixelFormat::R16F:           return GL_FLOAT;
	case PixelFormat::R32F:           return GL_FLOAT;
	case PixelFormat::RG8F:           return GL_FLOAT;
	case PixelFormat::RG16F:          return GL_FLOAT;
	case PixelFormat::RG32F:          return GL_FLOAT;
	case PixelFormat::R11G11B10F:     return GL_FLOAT;
	case PixelFormat::Depth:          return GL_UNSIGNED_BYTE;
	case PixelFormat::DepthStencil:   return GL_UNSIGNED_INT_24_8;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Pixel type is not supported");
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertBufferUsage(BufferUsage usage)
{
	switch (usage)
	{
	case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
	case BufferUsage::StaticDraw:  return GL_STATIC_DRAW;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Pixel type is not supported");
		return 0;
	}
	return 0;
}

uint32_t OpenGLTypes::ConvertShaderDataTypeCount(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:     return 1;
	case ShaderDataType::Float2:    return 2;
	case ShaderDataType::Float3:    return 3;
	case ShaderDataType::Float4:    return 4;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Shader data type is not supported");
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:     return 4;
	case ShaderDataType::Float2:    return 8;
	case ShaderDataType::Float3:    return 12;
	case ShaderDataType::Float4:    return 16;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Shader data type is not supported");
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertShaderDataTypeType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:     return GL_FLOAT;
	case ShaderDataType::Float2:    return GL_FLOAT;
	case ShaderDataType::Float3:    return GL_FLOAT;
	case ShaderDataType::Float4:    return GL_FLOAT;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Shader data type is not supported");
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertBlendFactor(BlendFactor factor)
{
	switch (factor)
	{
	case BlendFactor::One:                 return GL_ONE;
	case BlendFactor::SourceAlpha:         return GL_SRC_ALPHA;
	case BlendFactor::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Blend factor is not supported");
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertShaderType(ShaderType type)
{
	switch (type)
	{
	case ShaderType::Vertex:   return GL_VERTEX_SHADER;
	case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
	case ShaderType::Pixel:    return GL_FRAGMENT_SHADER;
	case ShaderType::Compute:  return GL_COMPUTE_SHADER;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Shdare type is not supported")
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertDepthTestFunction(DepthTestFunction function)
{
	switch (function)
	{
	case DepthTestFunction::One:     return GL_ONE;
	case DepthTestFunction::Greater: return GL_GREATER;
	case DepthTestFunction::Lesser:  return GL_LESS;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Depth test function is not supported")
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertFace(Face face)
{
	switch (face)
	{
	case Face::Front: return GL_FRONT;
	case Face::Back:  return GL_BACK;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Face is not supported")
		return 0;
	}
}

uint32_t OpenGLTypes::ConverTextureType(TextureType type)
{
	switch (type)
	{
	case TextureType::Texture2D:      return GL_TEXTURE_2D;
	case TextureType::CubeTexture:    return GL_TEXTURE_CUBE_MAP;
	case TextureType::Texture2DArray: return GL_TEXTURE_2D_ARRAY;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Texture type is not supported")
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertBarrierType(BarrierType type)
{
	switch (type)
	{
	case BarrierType::AllBits:   return GL_ALL_BARRIER_BITS;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Barrier type is not supported")
		return 0;
	}
}

uint32_t OpenGLTypes::ConvertDrawMode(DrawMode mode)
{
	switch (mode)
	{
	case DrawMode::Triangles:  return GL_TRIANGLES;
	case DrawMode::Lines:      return GL_LINES;
	case DrawMode::LineStrip:  return GL_LINE_STRIP;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Draw mode is not supported")
		return 0;
	}
}
