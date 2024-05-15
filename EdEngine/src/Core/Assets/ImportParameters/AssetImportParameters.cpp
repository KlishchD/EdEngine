#include "AssetImportParameters.h"

void AssetImportParameters::Serialize(Archive& archive)
{
	Super::Serialize(archive);

	archive & Path;
}
