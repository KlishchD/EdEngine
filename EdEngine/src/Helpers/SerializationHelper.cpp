#include "SerializationHelper.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"

std::shared_ptr<Asset> SerializationHelper::SerializeAssetInternal(Archive& archive, std::shared_ptr<Asset> asset)
{
  if (archive.GetMode() == ArchiveMode::Read)
  {
    boost::uuids::uuid id = boost::uuids::nil_uuid();
    archive & id;

    return Engine::Get().GetManager<AssetManager>()->LoadAsset(id);
  }
  else
  {
    boost::uuids::uuid id = boost::uuids::nil_uuid();

    if (asset)
    {
      id = asset->GetId();
    }

    archive & id;

    return asset;
  }
}
