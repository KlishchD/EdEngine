#include "TextureImportParameters.h"

void TextureImportParameters::Serialize(Archive& archive)
{
	AssetImportParameters::Serialize(archive);

	archive & WrapS;
	archive & WrapT;

	archive & Format;
	archive & Filtering;
}

void Texture2DImportParameters::Serialize(Archive& archive)
{
	TextureImportParameters::Serialize(archive);

	archive & GenerateMipMaps;
}

void CubeTextureImportParameters::Serialize(Archive& archive)
{
	TextureImportParameters::Serialize(archive);

	archive & WrapR;
}
