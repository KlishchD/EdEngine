#include "TextureDescriptor.h"

TextureImportParameters* Texture2DDescriptor::GetImportParameters() const
{
    return (TextureImportParameters*) &ImportParameters;
}

TextureData* Texture2DDescriptor::GetData() const
{
    return (TextureData*) &Data;
}

TextureImportParameters* CubeTextureDescriptor::GetImportParameters() const
{
    return (TextureImportParameters*) &ImportParameters;
}

TextureData* CubeTextureDescriptor::GetData() const
{
    return (TextureData*) &Data;
}
