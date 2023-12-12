#include "TextureImportParameters.h"

TextureImportParameters::TextureImportParameters(const std::string& path, WrapMode wrapS, WrapMode wrapT, PixelFormat format, FilteringMode filtering) : Path(path), WrapS(wrapS), WrapT(wrapT), Format(format), Filtering(filtering)
{
}

CubeTextureImportParameters::CubeTextureImportParameters(const std::string& path, WrapMode wrapS, WrapMode wrapT, WrapMode wrapR, PixelFormat format, FilteringMode filtering): TextureImportParameters(path, wrapS, wrapT, format, filtering)
{

}

Texture2DImportParameters::Texture2DImportParameters(const std::string& path, WrapMode wrapS, WrapMode wrapT, PixelFormat format, FilteringMode filtering): TextureImportParameters(path, wrapS, wrapT, format, filtering)
{
}
