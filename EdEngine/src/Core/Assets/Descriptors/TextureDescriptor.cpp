#include "TextureDescriptor.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Texture2DDescriptor)
BOOST_CLASS_EXPORT_IMPLEMENT(CubeTextureDescriptor)

const TextureImportParameters* Texture2DDescriptor::GetImportParameters() const
{
    return &ImportParameters;
}

const TextureData* Texture2DDescriptor::GetData() const
{
    return &Data;
}

bool Texture2DDescriptor::HasData() const
{
    return Data.GetData();
}

const TextureImportParameters* CubeTextureDescriptor::GetImportParameters() const
{
    return &ImportParameters;
}

const TextureData* CubeTextureDescriptor::GetData() const
{
    return &Data;
}

bool CubeTextureDescriptor::HasData() const
{
    return Data.GetData();
}

const TextureImportParameters* Texture2DArrayDescriptor::GetImportParameters() const
{
    return &ImportParameters;
}

const TextureData* Texture2DArrayDescriptor::GetData() const
{
    return &Data;
}

bool Texture2DArrayDescriptor::HasData() const
{
    return false;
}
