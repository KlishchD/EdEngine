#include "EdCore.h"

DataSource::DataSource(const Path& filepath, SerializationMode mode) : m_SourceType(FileSourceType), m_Mode(mode)
{
    const c8* fopenMode;

    if (mode == SerializationMode::Read)
    {
        fopenMode = "rb";
    }
    else
    {
        fopenMode = "wb+";
    }

    m_Source = static_cast<void*>(fopen(filepath.Get(), fopenMode));

    ED_ASSERT(m_Source, "Failed to open/create a file [{}] in mode [{}].", filepath.Get(), fopenMode);
}

DataSource::DataSource(void* buffer, SerializationMode mode) : m_SourceType(MemorySourceType), m_Mode(mode), m_Source(buffer)
{
    ED_ASSERT(buffer, "Source buffer can not be nullptr.");
}

DataSource::DataSource(DataSource&& source)
{
    (*this) = std::move(source);
}

DataSource::DataSource(const DataSource& source)
{
    ED_ASSERT(0, "Copy constructor is not supported.");
}

DataSource& DataSource::operator=(const DataSource& source)
{
    ED_ASSERT(0, "Copy operator is not supported.");
    return *this;
}

DataSource& DataSource::operator=(DataSource&& source)
{
    m_SourceType = source.m_SourceType;
    m_Mode = source.m_Mode;
    m_Source = source.m_Source;

    source.m_SourceType = 0;
    source.m_Source = nullptr;

    return *this;
}

void DataSource::Read(void* destination, u32 size)
{
    ED_ASSERT(m_Mode == SerializationMode::Read, "Can not read in a write mode");

    if (m_SourceType == FileSourceType)
    {
        fread(destination, size, 1, static_cast<FILE*>(m_Source));
    }
    else
    {
        memcpy(destination, m_Source, size);
        m_Source = static_cast<void*>(static_cast<u8*>(m_Source) + size);
    }
}

void DataSource::Write(const void* data, u32 size)
{
    ED_ASSERT(m_Mode == SerializationMode::Write, "Can not write in a read mode");

    if (m_SourceType == FileSourceType)
    {
        fwrite(data, size, 1, static_cast<FILE*>(m_Source));
    }
    else
    {
        memcpy(m_Source, data, size);
        m_Source = static_cast<void*>(static_cast<u8*>(m_Source) + size);
    }
}

DataSource::~DataSource()
{
    if (m_SourceType == FileSourceType)
    {
        fclose(static_cast<FILE*>(m_Source));
    }
}
