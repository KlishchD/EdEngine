#include "AssetImportParameters.h"

void AssetImportParameters::Serialize(Archive& archive)
{
	archive & Path;
}
