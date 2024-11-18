#include "SerializationHelper.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"

std::shared_ptr<Asset> SerializationHelper::SerializeAssetInternal(Archive& archive, std::shared_ptr<Asset> asset)
{
    uint64_t id;

	if (archive.GetMode() == ArchiveMode::Read)
	{
		archive & id;

		return Engine::Get().GetManager<AssetManager>()->LoadAsset(id);
	}
	else
	{
		if (asset)
		{
			id = asset->GetId();
		}

		archive & id;

		return asset;
	}
}
