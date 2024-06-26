#include "Serializable.h"
#include <fstream>

Archive::Archive(const std::string& path, ArchiveMode mode) : m_Mode(mode), m_Path(path)
{
	if (mode == ArchiveMode::Read)
	{
		m_InputFile = std::ifstream(path, std::ios::binary);
		m_Input = std::make_unique<boost::archive::text_iarchive>(m_InputFile);
	}
	else
	{
		m_OutputFile = std::ofstream(path, std::ios::binary);
		m_Output = std::make_unique<boost::archive::text_oarchive>(m_OutputFile);
	}
}

ArchiveMode Archive::GetMode() const
{
	return m_Mode;
}

const std::string& Archive::GetPath() const
{
	return m_Path;
}

void Serializable::Serialize(Archive& archive)
{
	archive & m_Version;
}