#include "TextureImportParameters.h"

void TextureImportParameters::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & WrapS;
	archive & WrapT;

	archive & Format;
	archive & Filtering;
}

void Texture2DImportParameters::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & GenerateMipMaps;
}

void CubeTextureImportParameters::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & WrapR;
}
