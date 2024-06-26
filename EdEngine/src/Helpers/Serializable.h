#pragma once

#include <fstream>
#include "Core/Objects/Class.h"

class GameObject;

class Asset;
enum class AssetType : uint8_t;

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

template <typename T>
concept IsArchive = std::is_base_of_v<Archive, T>;

template <typename T>
concept HasSerializeFunction = requires(T t, Archive& archive) {
	t.Serialize(archive);
};

class Archive
{
public:
	Archive(const std::string& path, ArchiveMode mode);
	
	ArchiveMode GetMode() const;
	
	// TODO: Add using for base types ;)
	
	const std::string& GetPath() const;

	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& Serialize(E& value);

	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& operator&(E& value);

	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& Serialize(E&& value);
	
	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& operator&(E&& value);

	
	template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
	Archive& Serialize(E&& value);
	
	template<typename E> requires(!std::is_base_of_v<GameObject, E>&& HasSerializeFunction<E>)
	Archive& operator&(E&& value);
	
	template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
	Archive& Serialize(E& value);

	template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
	Archive& operator&(E& value);

	
	template<typename E> requires(std::is_base_of_v<Asset, E>)
	Archive& Serialize(std::shared_ptr<E>& value, bool bLoadData);

	template<typename E> requires(std::is_base_of_v<Asset, E>)
	Archive& operator&(std::shared_ptr<E>& value);


	template<typename E> requires(std::is_base_of_v<GameObject, E> && !std::is_base_of_v<Asset, E>)
	Archive& Serialize(std::shared_ptr<E>& value);

	template<typename E> requires(std::is_base_of_v<GameObject, E> && !std::is_base_of_v<Asset, E>)
	Archive& operator&(std::shared_ptr<E>& value);


	template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
	Archive& Serialize(std::shared_ptr<E>& value);

	template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
	Archive& operator&(std::shared_ptr<E>& value);


	template<typename E>
	Archive& Serialize(std::vector<std::shared_ptr<E>>& values);

	template<typename E>
	Archive& operator&(std::vector<std::shared_ptr<E>>& values);

	template<typename E>
	Archive& Serialize(std::vector<std::shared_ptr<E>>&& values);

	template<typename E>
	Archive& operator&(std::vector<std::shared_ptr<E>>&& values);


	template<typename E> requires(HasSerializeFunction<E>)
	Archive& Serialize(std::vector<E>& values);

	template<typename E> requires(HasSerializeFunction<E>)
	Archive& operator&(std::vector<E>& values);

	template<typename E> requires(HasSerializeFunction<E>)
	Archive& Serialize(std::vector<E>&& values);

	template<typename E> requires(HasSerializeFunction<E>)
	Archive& operator&(std::vector<E>&& values);


	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& Serialize(std::vector<E>& values);

	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& operator&(std::vector<E>& values);

	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& Serialize(std::vector<E>&& values);

	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& operator&(std::vector<E>&& values);


	Archive& Serialize(std::string& value);

	Archive& operator&(std::string& value);
	
	Archive& Serialize(std::string&& value);

	Archive& operator&(std::string&& value);


	template<typename E> requires(HasSerializeFunction<E>)
	Archive& Serialize(E*& values, uint32_t& size);

	template<typename E> requires(!HasSerializeFunction<E>)
	Archive& Serialize(E*& values, uint32_t& size);


private:
	ArchiveMode m_Mode;
	
	std::string m_Path;
	
	std::ifstream m_InputFile;
	std::ofstream m_OutputFile;
};

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::Serialize(E& value)
{
	if (m_Mode == ArchiveMode::Read)
	{
		m_InputFile.read(reinterpret_cast<char*>(&value), sizeof(E));
	}
	else
	{
		m_OutputFile.write(reinterpret_cast<char*>(&value), sizeof(E));
	}

    return *this;
}

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::operator&(E& value)
{
	return Serialize(value);
}

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::Serialize(E&& value)
{
	ED_ASSERT(m_Mode == ArchiveMode::Write, "Can only write r-value");

	m_OutputFile.write(reinterpret_cast<char*>(&value), sizeof(value));

	return *this;
}

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::operator&(E&& value)
{
    return Serialize(std::move(value));
}

template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
Archive& Archive::operator&(E&& value)
{
    return Serialize(std::move(value));
}

template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
Archive& Archive::Serialize(E& value)
{
    value.Serialize(*this);
    return *this;
}

template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
Archive& Archive::operator&(E& value)
{
    return Serialize(value);
}

template<typename E> requires(std::is_base_of_v<Asset, E>)
Archive& Archive::Serialize(std::shared_ptr<E>& value, bool bLoadData)
{
	std::shared_ptr<GameObject> object = std::static_pointer_cast<GameObject>(value);
	Serialize<GameObject>(object);
	value = std::static_pointer_cast<E>(object);

	if (bLoadData || m_Mode == ArchiveMode::Write)
	{
		value->SerializeData(*this);
	}

    return *this;
}

template<typename E> requires(std::is_base_of_v<Asset, E>)
Archive& Archive::operator&(std::shared_ptr<E>& value)
{
	return Serialize(value, true);
}

template<typename E> requires(std::is_base_of_v<GameObject, E> && !std::is_base_of_v<Asset, E>)
Archive& Archive::Serialize(std::shared_ptr<E>& value)
{
    if (m_Mode == ArchiveMode::Read)
    {
        std::string className;
        Serialize(className);

        if (!value)
        {
            const Class* clazz = ObjectFactory::GetClass(className);

			ED_ASSERT(clazz, "Cannot find serialized class {}", className);

            value = clazz->Create<E>();
        }
    }
    else
    {
		ED_ASSERT(value, "Cannot serialize nullptr");

        Serialize(value->GetClass().GetName());
    }

    value->Serialize(*this);

    return *this;
}

template<typename E> requires(std::is_base_of_v<GameObject, E> && !std::is_base_of_v<Asset, E>)
Archive& Archive::operator&(std::shared_ptr<E>& value)
{
    return Serialize(value);
}

template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
Archive& Archive::Serialize(std::shared_ptr<E>& value)
{
    if (m_Mode == ArchiveMode::Read)
    {
        if (!value)
        {
            value = std::make_shared<E>();
        }
    }
    else
    {
        ED_ASSERT(value, "Cannot serialize nullptr")
    }

    value->Serialize(*this);

	return *this;
}

template<typename E> requires(!std::is_base_of_v<GameObject, E> && HasSerializeFunction<E>)
Archive& Archive::operator&(std::shared_ptr<E>& value)
{
	return Serialize(value);
}


template<typename E>
Archive& Archive::Serialize(std::vector<std::shared_ptr<E>>& values)
{
    uint32_t size = values.size();

    Serialize(size);

    values.resize(size);

    for (int32_t i = 0; i < size; ++i)
    {
        std::shared_ptr<GameObject> object = std::static_pointer_cast<GameObject>(values[i]);
        Serialize<GameObject>(object);
		values[i] = std::static_pointer_cast<E>(object);
    }

    return *this;
}

template<typename E>
Archive& Archive::operator&(std::vector<std::shared_ptr<E>>& values)
{
    return Serialize(values);
}

template<typename E>
Archive& Archive::Serialize(std::vector<std::shared_ptr<E>>&& values)
{
    ED_ASSERT(m_Mode == ArchiveMode::Write, "Can not read to r-value");

    uint32_t size = values.size();

    Serialize(size);

    for (int32_t i = 0; i < size; ++i)
    {
        values[i]->Serialize(*this);
    }

    return *this;
}

template<typename E>
Archive& Archive::operator&(std::vector<std::shared_ptr<E>>&& values)
{
    return Serialize(values);
}

template<typename E> requires(HasSerializeFunction<E>)
Archive& Archive::Serialize(std::vector<E>& values)
{
	uint32_t size = values.size();

	Serialize(size);

    values.resize(size);

	for (int32_t i = 0; i < size; ++i)
	{
        Serialize(values[i]);
	}

	return *this;
}

template<typename E> requires(HasSerializeFunction<E>)
Archive& Archive::operator&(std::vector<E>& values)
{
	return Serialize(values);
}

template<typename E> requires(HasSerializeFunction<E>)
Archive& Archive::Serialize(std::vector<E>&& values)
{
	ED_ASSERT(m_Mode == ArchiveMode::Write, "Can not read to r-value");

	uint32_t size = values.size();

	Serialize(size);

    for (int32_t i = 0; i < size; ++i)
    {
        Serialize(values[i]);
    }

    return *this;
}

template<typename E> requires(HasSerializeFunction<E>)
Archive& Archive::operator&(std::vector<E>&& values)
{
    return Serialize(values);
}

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::Serialize(std::vector<E>& values)
{
    uint32_t size = values.size();

    Serialize(size);

    values.resize(size);

	if (m_Mode == ArchiveMode::Read)
	{
		m_InputFile.read(reinterpret_cast<char*>(&values[0]), sizeof(E) * size);
	}
	else
	{
		m_OutputFile.write(reinterpret_cast<char*>(&values[0]), sizeof(E) * size);
	}

    return *this;
}

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::operator&(std::vector<E>& values)
{
    return Serialize(values);
}

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::Serialize(std::vector<E>&& values)
{
    ED_ASSERT(m_Mode == ArchiveMode::Write, "Can not read to r-value");

    uint32_t size = values.size();

    Serialize(size);

    m_OutputFile.write(reinterpret_cast<char*>(&values[0]), sizeof(E) * size);

    return *this;
}

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::operator&(std::vector<E>&& values)
{
    return Serialize(values);
}

template<typename E> requires(HasSerializeFunction<E>)
Archive& Archive::Serialize(E*& value, uint32_t& size)
{
	Serialize(size);

	if (value == nullptr && m_Mode == ArchiveMode::Read)
	{
		value = (E*)malloc(size * sizeof(E));
	}

	for (uint32_t i = 0; i < size; ++i)
	{
		Serialize(value[i]);
	}

	return *this;
}

template<typename E> requires(!HasSerializeFunction<E>)
Archive& Archive::Serialize(E*& value, uint32_t& size)
{
    Serialize(size);

	if (m_Mode == ArchiveMode::Read)
	{
        if (value == nullptr)
        {
            value = (E*)malloc(size * sizeof(E));
        }

		m_InputFile.read(reinterpret_cast<char*>(value), sizeof(E) * size);
	}
	else
	{
		ED_ASSERT(value, "Can not write nullptr");

		m_OutputFile.write(reinterpret_cast<char*>(value), sizeof(E) * size);
	}

    return *this;
}