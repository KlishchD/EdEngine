#include "AssetFactory.h"
#include "Core/Macros.h"

std::shared_ptr<Asset> AssetTypeFactory::Create(AssetType type)
{
	return m_Factories[type]->Create();
}

std::shared_ptr<Asset> AssetTypeFactory::Create(AssetType type, const std::string& path)
{
	Archive archive(path, ArchiveMode::Write);
	return m_Factories[type]->Create(archive);
}

std::shared_ptr<Asset> AssetTypeFactory::Load(const std::string& path, bool bShouldLoadData)
{
	Archive archive(path, ArchiveMode::Read);
	return Load(archive, bShouldLoadData);
}

std::shared_ptr<Asset> AssetTypeFactory::Load(Archive& arcive, bool bShouldLoadData)
{
	ED_ASSERT(arcive.GetMode() == ArchiveMode::Read, "Archive must be open for reading")

	Archive tmpArchive(arcive.GetPath(), ArchiveMode::Read);
	AssetType type = AssetType::None;
	tmpArchive & type;
	
	return m_Factories[type]->Load(arcive, bShouldLoadData);
}

AssetTypeFactory::AssetTypeFactory(std::shared_ptr<AssetManager> manager) : m_Manager(manager)
{

}

AssetFactory::AssetFactory(std::shared_ptr<AssetManager> manager) : m_Manager(manager)
{

}

AssetType AssetFactory::GetType()
{
	return AssetType::None;
}
