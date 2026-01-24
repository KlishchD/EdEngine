#include "EdRenderApiPrivate.h"
#include "RenderTypes.h"

u32 RenderTypes::GetChannelNumber(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::R8F:          return 1;
    case PixelFormat::R16F:         return 1;
    case PixelFormat::R32F:         return 1;

    case PixelFormat::RG8F:         return 2;
    case PixelFormat::RG16F:        return 2;
    case PixelFormat::RG32F:        return 2;

    case PixelFormat::RGB8F:        return 3;
    case PixelFormat::RGB16F:       return 3;
    case PixelFormat::RGB32F:       return 3;

    case PixelFormat::RGBA8F:       return 4;
    case PixelFormat::RGBA16F:      return 4;
    case PixelFormat::RGBA32F:      return 4;

    case PixelFormat::SRGBA8F:      return 4;

    case PixelFormat::R11G11B10F:   return 1;

    case PixelFormat::Depth:        return 1;
    case PixelFormat::DepthStencil: return 1;

    default:
        ED_ASSERT_CONTEXT(RenderAPI, 0, "Pixel format is not supported");
        return 0;
    }
}

u32 RenderTypes::GetPixelSize(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::R8F:          return 1 * sizeof(u8);
    case PixelFormat::R16F:         return 1 * sizeof(u16);
    case PixelFormat::R32F:         return 1 * sizeof(u32);

    case PixelFormat::RG8F:         return 2 * sizeof(u8);
    case PixelFormat::RG16F:        return 2 * sizeof(u16);
    case PixelFormat::RG32F:        return 2 * sizeof(u32);

    case PixelFormat::RGB8F:        return 3 * sizeof(u8);
    case PixelFormat::RGB16F:       return 3 * sizeof(u16);
    case PixelFormat::RGB32F:       return 3 * sizeof(u32);

    case PixelFormat::RGBA8F:       return 4 * sizeof(u8);
    case PixelFormat::RGBA16F:      return 4 * sizeof(u16);
    case PixelFormat::RGBA32F:      return 4 * sizeof(u32);

    case PixelFormat::SRGBA8F:      return 4 * sizeof(u8);

    case PixelFormat::R11G11B10F:   return 1 * sizeof(u32);

    case PixelFormat::Depth:        return 1 * sizeof(u32);
    case PixelFormat::DepthStencil: return 1 * sizeof(u32);

    default:
        ED_ASSERT_CONTEXT(RenderAPI, 0, "Pixel format is not supported");
        return 0;
    }
}

u32 RenderTypes::ConvertShaderDataTypeCount(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float:     return 1;
    case ShaderDataType::Float2:    return 2;
    case ShaderDataType::Float3:    return 3;
    case ShaderDataType::Float4:    return 4;
    default:
        ED_ASSERT_CONTEXT(RenderAPI, 0, "Shader data type is not supported");
        return 0;
    }
}

u32 RenderTypes::ConvertShaderDataTypeSize(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float:     return 4;
    case ShaderDataType::Float2:    return 8;
    case ShaderDataType::Float3:    return 12;
    case ShaderDataType::Float4:    return 16;
    default:
        ED_ASSERT_CONTEXT(RenderAPI, 0, "Shader data type is not supported");
        return 0;
    }
}

f32 RenderTypes::ConvertRenderTargetSizePolicy(RenderTargetSizePolicy policy)
{
	switch (policy)
	{
	case RenderTargetSizePolicy::Upscale16:   return 16.0f;
	case RenderTargetSizePolicy::Upscale8:    return 8.0f;
	case RenderTargetSizePolicy::Upscale4:    return 4.0f;
	case RenderTargetSizePolicy::Upscale2:    return 2.0f;
	case RenderTargetSizePolicy::Full:        return 1.0f;
	case RenderTargetSizePolicy::Downscale2:  return 0.5f;
	case RenderTargetSizePolicy::Downscale4:  return 0.25f;
	case RenderTargetSizePolicy::Downscale8:  return 0.125f;
	case RenderTargetSizePolicy::Downscale16: return 0.0625f;
	default:
		ED_ASSERT(0, "Renderer does not support provided render target size policy.");
	}

	return 0.0f;
}

PixelFormat RenderTypes::CompressType(PixelFormat format, u32 channals)
{
	ED_ASSERT(channals > 0 && channals <= 4, "Can only handle [1;4] channels.");

	if (channals == 3)
	{
		// Some apis do not handle 3 channals well.
		channals = 4;
	}

	constexpr u32 formatBlocksSize = static_cast<u32>(PixelFormat::R32F) + 1;
	u32 local = static_cast<u32>(format) % formatBlocksSize;
	u32 group = static_cast<u32>(format) / formatBlocksSize;

	return static_cast<PixelFormat>(group * formatBlocksSize + local);
}
