#include "Asset.h"
#include "Core/Macros.h"

Asset::Asset(const std::string& name) : m_Id(UUIDs::random_generator()()), m_Name(name)
{

}

UUID Asset::GetId() const
{
	return m_Id;
}

std::string Asset::GetName() const
{
	return m_Name;
}

AssetType Asset::GetType() const
{
	return AssetType::None;
}

bool Asset::HasData() const
{
	return m_bHasData;
}

void Asset::ClaimData()
{
	++m_DataClaims;
}

void Asset::UnclaimData()
{
	--m_DataClaims;
}

void Asset::MarkDirty()
{
	m_bIsDirty = true;
    // TODO: Maybe add define for editor only ?)
}

bool Asset::IsDirty() const
{
    return m_bIsDirty;
}

void Asset::SetShouldLoadData(bool status)
{
	m_bShouldHaveData = status;
}

bool Asset::ShouldHaveData() const
{
	return m_bShouldHaveData;
}

void Asset::SetImportParameters(std::shared_ptr<AssetImportParameters> parameters)
{
	m_ImportParameters = parameters;
	ResetState();
}

std::shared_ptr<AssetImportParameters> Asset::GetImportParameters() const
{
	return m_ImportParameters;
}

void Asset::ResetState()
{

}

void Asset::Serialize(Archive& archive)
{
	if (archive.GetMode() == ArchiveMode::Write)
	{
		archive & GetType();
	}

	Serializable::Serialize(archive);

	archive & m_Id;
	archive & m_Name;

	archive & m_ImportParameters;
}

void Asset::SerializeData(Archive& archive)
{
	m_bHasData = true;
	m_bIsDirty = false;
}

void Asset::FreeData()
{
}
