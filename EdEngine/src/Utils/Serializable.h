#pragma once

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_serialize.hpp>

#include <boost/serialization/serialization.hpp>

#include <fstream>

#include "Core/Macros.h"

using UUID = boost::uuids::uuid;
namespace UUIDs = boost::uuids;

class Asset;

class Serializable
{
public:
	virtual void Serialize(class Archive& archive);

	uint32_t GetVersion() const
	{
		return m_Version;
	}

protected:
	uint32_t m_Version = 0;
};


enum class ArchiveMode : uint8_t
{
	Read,
	Write
};

class Archive
{
public:
	Archive(const std::string& path, ArchiveMode mode);

	ArchiveMode GetMode() const;

	// TODO: Add using for base types ;)

	const std::string& GetPath() const;

	template<typename E> requires(std::is_enum_v<E> || std::is_integral_v<E>)
	Archive& operator&(E&& value);

	template<typename E> requires(std::is_enum_v<E> || std::is_integral_v<E>)
	Archive& operator&(E& value);

	template<typename E>
	Archive& operator&(std::vector<E>& values);

	template<typename E> requires(std::is_base_of_v<Asset, E>)
	Archive& operator&(std::vector<std::shared_ptr<E>>& values);

	template<typename E> requires(!std::is_base_of_v<Asset, E> && std::is_base_of_v<Serializable, E>)
	Archive& operator&(std::vector<std::shared_ptr<E>>& values);

	template<typename E> requires(!std::is_enum_v<E> && !std::is_integral_v<E> && !std::is_base_of_v<Serializable, E>)
	Archive& operator&(E& value);

	template<typename E> requires(std::is_base_of_v<Serializable, E>)
	Archive& operator&(std::shared_ptr<E>& serializable);

private:
	ArchiveMode m_Mode;

	std::string m_Path;

	std::ifstream m_InputFile;
	std::ofstream m_OutputFile;

	std::unique_ptr<boost::archive::text_iarchive> m_Input;
	std::unique_ptr<boost::archive::text_oarchive> m_Output;
};


template<typename E> requires(std::is_enum_v<E> || std::is_integral_v<E>)
Archive& Archive::operator&(E&& value)
{
	ED_ASSERT(m_Mode == ArchiveMode::Write, "Cannot change r-value")

	*m_Output << value;

	return *this;
}

template<typename E> requires(std::is_enum_v<E> || std::is_integral_v<E>)
Archive& Archive::operator&(E& value)
{
	if (m_Mode == ArchiveMode::Read)
	{
		*m_Input >> value;
	}
	else
	{
		*m_Output << value;
	}

	return *this;
}

template<typename E>
Archive& Archive::operator&(std::vector<E>& values)
{
	if (m_Mode == ArchiveMode::Read)
	{
		int32_t size = 0;

		(*this) & size;

		for (int32_t i = 0; i < size; ++i)
		{
			E value {};
			(*this) & value;
			values.push_back(value);
		}
	}
	else
	{
		(*this) & values.size();

		for (E& value : values)
		{
			(*this) & value;
		}
	}

	return *this;
}

template<typename E> requires(std::is_base_of_v<Asset, E>)
Archive& Archive::operator&(std::vector<std::shared_ptr<E>>& values)
{
	if (m_Mode == ArchiveMode::Read)
	{
		if (values.empty())
		{
			int32_t size = 0;

			(*this) & size;

			for (int32_t i = 0; i < size; ++i)
			{
				std::shared_ptr<E> value = std::make_shared<E>();

				value->Serialize(*this);
				value->SerializeData(*this);

				values.push_back(value);
			}
		}
		else
		{
			for (std::shared_ptr<E> value : values)
			{
				value->SerializeData(*this);
			}
		}
	}
	else
	{
		(*this) & values.size();

		for (std::shared_ptr<E>& value : values)
		{
			value->Serialize(*this);
			value->SerializeData(*this);
		}
	}

	return *this;
}

template<typename E> requires(!std::is_base_of_v<Asset, E>&& std::is_base_of_v<Serializable, E>)
Archive& Archive::operator&(std::vector<std::shared_ptr<E>>& values)
{
	if (m_Mode == ArchiveMode::Read)
	{
		int32_t size = 0;

		(*this) & size;

		for (int32_t i = 0; i < size; ++i)
		{
			std::shared_ptr<E> value = std::make_shared<E>();

			value->Serialize(*this);

			values.push_back(value);
		}
	}
	else
	{
		(*this) & values.size();

		for (std::shared_ptr<E>& value : values)
		{
			ED_ASSERT(value, "Cannot write nullptr")
			value->Serialize(*this);
		}
	}

	return *this;
}

template<typename E> requires(!std::is_enum_v<E> && !std::is_integral_v<E> && !std::is_base_of_v<Serializable, E>)
Archive& Archive::operator&(E& value)
{
	if (m_Mode == ArchiveMode::Read)
	{
		*m_Input >> value;
	}
	else
	{
		*m_Output << value;
	}

	return *this;
}

template<typename E> requires(std::is_base_of_v<Serializable, E>)
Archive& Archive::operator&(std::shared_ptr<E>& serializable)
{
	if (m_Mode == ArchiveMode::Write)
	{
		ED_ASSERT(serializable, "Cannot write nullptr")
		serializable->Serialize(*this);
	}
	else
	{
		serializable = std::make_shared<E>();
		serializable->Serialize(*this);
	}

	return *this;
}
