﻿#include "SerializationHelper.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"

std::shared_ptr<Asset> SerializationHelper::SerializeAssetInternal(Archive& archive, std::shared_ptr<Asset> asset)
{
	if (archive.GetMode() == ArchiveMode::Read)
	{
		UUID id = UUIDs::nil_uuid();
		archive & id;

		return Engine::Get().GetManager<AssetManager>()->LoadAsset(id);
	}
	else
	{
		UUID id = UUIDs::nil_uuid();

		if (asset)
		{
			id = asset->GetId();
		}

		archive & id;

		return asset;
	}
}
