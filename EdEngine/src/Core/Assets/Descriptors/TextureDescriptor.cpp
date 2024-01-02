#include "TextureDescriptor.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Texture2DDescriptor)
BOOST_CLASS_EXPORT_IMPLEMENT(CubeTextureDescriptor)

TextureImportParameters* Texture2DDescriptor::GetImportParameters() const
{
    return (TextureImportParameters*) &ImportParameters;
}

TextureData* Texture2DDescriptor::GetData() const
{
    return (TextureData*) &Data;
}

bool Texture2DDescriptor::HasData() const
{
    return Data.Data != nullptr;
}

TextureImportParameters* CubeTextureDescriptor::GetImportParameters() const
{
    return (TextureImportParameters*) &ImportParameters;
}

TextureData* CubeTextureDescriptor::GetData() const
{
    return (TextureData*) &Data;
}

bool CubeTextureDescriptor::HasData() const
{
    return Data.Data[0] != nullptr;
}
