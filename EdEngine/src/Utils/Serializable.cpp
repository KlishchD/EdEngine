#include "Serializable.h"
#include <fstream>

Archive::Archive(const std::string& path, ArchiveMode mode) : m_Mode(mode), m_Path(path)
{
	if (mode == ArchiveMode::Read)
	{
		m_InputFile = std::ifstream(path, std::ios::binary);
	}
	else
	{
		m_OutputFile = std::ofstream(path, std::ios::binary);
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

Archive& Archive::Serialize(std::string& value)
{
    uint32_t size = value.size();

    Serialize(size);

    value.resize(size);

    if (m_Mode == ArchiveMode::Read)
    {
        m_InputFile.read(&value[0], size);
    }
    else
    {
        m_OutputFile.write(&value[0], size);
    }

    return *this;
}

Archive& Archive::operator&(std::string& value)
{
    return Serialize(value);
}

Archive& Archive::Serialize(std::string&& value)
{
    ED_ASSERT(m_Mode == ArchiveMode::Write, "Can not read to r-value");

    uint32_t size = value.size();

    Serialize(size);

    m_OutputFile.write(&value[0], size);

    return *this;
}

Archive& Archive::operator&(std::string&& value)
{
    return Serialize(std::move(value));
}

void Serializable::Serialize(Archive& archive)
{
	archive & m_Version;
}