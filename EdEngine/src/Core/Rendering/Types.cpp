#include "Types.h"
#include "Core/Macros.h"

uint32_t Types::GetChannelNumber(PixelFormat format)
{
	switch (format)
	{
	case PixelFormat::RGB8F:      return 3;
	case PixelFormat::RGBA8F:     return 4;
	case PixelFormat::SRGB8F:     return 3;
	case PixelFormat::SRGBA8F:    return 4;
	case PixelFormat::RGB16F:     return 3;
	case PixelFormat::RGBA16F:    return 4;
	case PixelFormat::RGB32F:     return 3;
	case PixelFormat::R8F:        return 1;
	case PixelFormat::R16F:       return 1;
	case PixelFormat::R32F:       return 1;
	case PixelFormat::RG8F:       return 2;
	case PixelFormat::RG16F:      return 2;
	case PixelFormat::RG32F:      return 2;
	case PixelFormat::R11G11B10F: return 3;
	default:
		ED_LOG(Types, warn, "Cannot calculate channels count")
	}
    return 0;
}

uint32_t Types::GetPixelSize(PixelFormat format)
{
	switch (format)
	{
	case PixelFormat::RGB8F:      return 3 * sizeof(uint8_t);
	case PixelFormat::RGBA8F:     return 4 * sizeof(uint8_t);
	case PixelFormat::SRGB8F:     return 3 * sizeof(uint8_t);
	case PixelFormat::SRGBA8F:    return 4 * sizeof(uint8_t);
	case PixelFormat::RGB16F:     return 3 * sizeof(uint16_t);
	case PixelFormat::RGBA16F:    return 4 * sizeof(uint16_t);
	case PixelFormat::RGB32F:     return 3 * sizeof(uint32_t);
	case PixelFormat::R8F:        return     sizeof(uint8_t);
	case PixelFormat::R16F:       return     sizeof(uint16_t);
	case PixelFormat::R32F:       return     sizeof(uint32_t);
	case PixelFormat::RG8F:       return 2 * sizeof(uint8_t);
	case PixelFormat::RG16F:      return 2 * sizeof(uint16_t);
	case PixelFormat::RG32F:      return 2 * sizeof(uint32_t);
	case PixelFormat::R11G11B10F: return     sizeof(uint32_t);
	default:
		ED_LOG(Types, warn, "Cannot calculate pixel size")
	}
	return 0;
}
