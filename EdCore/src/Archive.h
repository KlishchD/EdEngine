#pragma once

enum class SerializationMode : u8
{
    Read,
    Write
};

template <FreeListAllocatorGetter Allocator = &SerializationDataAllocator>
class Archive;
class Path;

class DataSource
{
public:
    static inline u32 FileSourceType = 1;
    static inline u32 MemorySourceType = 2;
public:
    DataSource(const Path& filepath, SerializationMode mode);
    DataSource(void* buffer, SerializationMode mode);

    DataSource(DataSource&& source);
    DataSource& operator=(DataSource&& source);

    inline u32 GetSourceType() const { return m_SourceType; }
    inline SerializationMode GetMode() const { return m_Mode; }

    void Read(void* destination, u32 size);
    void Write(const void* data, u32 size);

    ~DataSource();
protected:
    DataSource(const DataSource& source);
    DataSource& operator=(const DataSource& source);
protected:
    u32 m_SourceType;
	SerializationMode m_Mode;
    void* m_Source;
};

// Major TODO: Need some additional safety for different allocator types,
// it is way to easy to add new allocator to archive and for rest of the
// systems not to work.

template <FreeListAllocatorGetter Allocator, typename T>
concept HasSerializeFunction = requires(T value, Archive<Allocator>& archive) {
	value.Serialize(archive);
};

template <FreeListAllocatorGetter Allocator, typename T>
concept HasSerializeDataFunction = requires(T value, Archive<Allocator>& archive) {
	value.SerializeData(archive);
	value.ShouldSerializeData(archive);
};

template <FreeListAllocatorGetter Allocator, typename T>
concept HasGetClassFunction = requires(T value, Archive<Allocator>& archive)
{
	value.GetClass(archive);
};

template <FreeListAllocatorGetter Allocator, typename T>
concept IsSerializableObject = HasSerializeFunction<Allocator, T> || HasSerializeDataFunction<Allocator, T> || HasGetClassFunction<Allocator, T>;

template <FreeListAllocatorGetter Allocator>
class Archive
{
public:
    Archive(DataSource&& source) : m_Source(std::move(source)) { }
    Archive(const Path& path, SerializationMode mode) : m_Source(path, mode) { }
	Archive(void* buffer, SerializationMode mode) : m_Source(buffer, mode) { }
	
	inline u32 GetSourceType() const { return m_Source.GetSourceType(); }
	inline SerializationMode GetMode() const { return m_Source.GetMode(); }

    template<typename E> requires(!std::is_pointer_v<E> && !std::is_const_v<E>)
	Archive<Allocator>& Serialize(E& value);

    template<typename E> requires(!std::is_pointer_v<E> && !std::is_const_v<E>)
	Archive<Allocator>& operator&(E& value) { return Serialize(value); }

	template<typename E> requires(!std::is_pointer_v<E> && std::is_const_v<E>)
	Archive<Allocator>& Serialize(E& value);

    template<typename E> requires(!std::is_pointer_v<E> && std::is_const_v<E>)
	Archive<Allocator>& operator&(E& value) { return Serialize(value); }


    template<typename E> requires(!std::is_pointer_v<E>)
	Archive<Allocator>& Serialize(E&& value);
	
	template<typename E> requires(!std::is_pointer_v<E>)
	Archive<Allocator>& operator&(E&& value) { return Serialize(std::move(value)); }
	
    template<typename E>
	Archive<Allocator>& Serialize(E** value);
	
	template<typename E>
	Archive<Allocator>& operator&(E** value) { return Serialize(value); }

    template<typename E, bool IsRValue>
    Archive<Allocator>& Serialize(E* values, u32 count, bool skipCount = false);

    template<typename E, bool IsRValue>
	Archive<Allocator>& Serialize(E** values, u32 count);

    template<typename E, bool IsRValue>
    Archive<Allocator>& Serialize(std::shared_ptr<E>* values, u32 count);

	
	template<typename E>
	Archive<Allocator>& Serialize(std::vector<std::shared_ptr<E>>& values)  { return Serialize(values.data(), values.size()); }

	template<typename E>
	Archive<Allocator>& operator&(std::vector<std::shared_ptr<E>>& values)  { return Serialize(values.data(), values.size()); }

	template<typename E>
	Archive<Allocator>& Serialize(std::vector<std::shared_ptr<E>>&& values) { return Serialize(values.data(), values.size()); }

	template<typename E>
	Archive<Allocator>& operator&(std::vector<std::shared_ptr<E>>&& values) { return Serialize(values.data(), values.size()); }


    template<typename E>
    Archive<Allocator>& Serialize(std::vector<E*>& values)  { return Serialize<E, false>(values.data(), values.size()); }

    template<typename E>
	Archive<Allocator>& operator&(std::vector<E*>& values)	 { return Serialize<E, false>(values.data(), values.size()); }

    template<typename E>
    Archive<Allocator>& Serialize(std::vector<E*>&& values) { return Serialize<E, true>(values.data(), values.size()); }

    template<typename E>
	Archive<Allocator>& operator&(std::vector<E*>&& values) { return Serialize<E, true>(values.data(), values.size()); }


	template<typename E>
    Archive<Allocator>& Serialize(std::vector<E>& values);

    template<typename E>
	Archive<Allocator>& operator&(std::vector<E>& values)  { return Serialize<E, true>(values.data(), values.size()); }

    template<typename E>
    Archive<Allocator>& Serialize(std::vector<E>&& values);

    template<typename E>
	Archive<Allocator>& operator&(std::vector<E>&& values) { return Serialize<E, false>(values.data(), values.size()); }

	Archive<Allocator>& Serialize(std::string& value);
	Archive<Allocator>& operator&(std::string& value)  { return Serialize(value); }
	
	Archive<Allocator>& Serialize(std::string&& value);
	Archive<Allocator>& operator&(std::string&& value) { return Serialize<c8, true>(value.data(), (u32)value.size()); }

private:
	DataSource m_Source;
};

template <FreeListAllocatorGetter Allocator>
template<typename E> requires(!std::is_pointer_v<E> && !std::is_const_v<E>)
Archive<Allocator>& Archive<Allocator>::Serialize(E& value)
{
	constexpr bool DumpBits = !IsSerializableObject<Allocator, E>;
	constexpr bool DoBasicSerialize = HasSerializeFunction<Allocator, E>;
	constexpr bool DoDataSerialize = HasSerializeDataFunction<Allocator, E>;
	constexpr bool DoClassSerialize = HasGetClassFunction<Allocator, E>;

	if constexpr (DumpBits)
	{
        if (GetMode() == SerializationMode::Read)
        {
            m_Source.Read(static_cast<void*>(&value), sizeof(E));
        }
        else
        {
            m_Source.Write(static_cast<void*>(&value), sizeof(E));
        }
	}
	else
	{
		if constexpr (DoClassSerialize)
		{
			if (GetMode() == SerializationMode::Write)
			{
				Serialize(value.GetClass().GetName());
			}
			else
			{
				std::string clazz;
				Serialize(clazz);
			}
		}

		if constexpr (DoBasicSerialize)
		{
			value.Serialize(*this);
		}

		if constexpr (DoDataSerialize)
		{
			if (value.ShouldSerializeData(*this))
			{
				value.SerializeData(*this);
			}
		}
	}
 
	return *this;
}

template <FreeListAllocatorGetter Allocator>
template<typename E> requires(!std::is_pointer_v<E>&& std::is_const_v<E>)
Archive<Allocator>& Archive<Allocator>::Serialize(E& value)
{
	ED_ASSERT(GetMode() == SerializationMode::Write, "Can not write to a const value.");

    constexpr bool DumpBits = !IsSerializableObject<Allocator, E>;
    constexpr bool DoBasicSerialize = HasSerializeFunction<Allocator, E>;
    constexpr bool DoDataSerialize = HasSerializeDataFunction<Allocator, E>;
    constexpr bool DoClassSerialize = HasGetClassFunction<Allocator, E>;

    if constexpr (DumpBits)
    {
		m_Source.Write(reinterpret_cast<const void*>(&value), sizeof(E));
    }
    else
    {
        if constexpr (DoClassSerialize)
        {
			Serialize(value.GetClass().GetName());
        }

        if constexpr (DoBasicSerialize)
        {
            value.Serialize(*this);
        }

        if constexpr (DoDataSerialize)
        {
            if (value.ShouldSerializeData(*this))
            {
                value.SerializeData(*this);
            }
        }
    }

    return *this;
}

template <FreeListAllocatorGetter Allocator>
template<typename E> requires(!std::is_pointer_v<E>)
Archive<Allocator>& Archive<Allocator>::Serialize(E&& value)
{
    ED_ASSERT(GetMode() == SerializationMode::Write, "Can not write to r-value");

    constexpr bool DumpBits = !IsSerializableObject<Allocator, E>;
    constexpr bool DoBasicSerialize = HasSerializeFunction<Allocator, E>;
    constexpr bool DoDataSerialize = HasSerializeDataFunction<Allocator, E>;
    constexpr bool DoClassSerialize = HasGetClassFunction<Allocator, E>;

    if constexpr (DumpBits)
    {
		m_Source.Write(reinterpret_cast<const void*>(&value), sizeof(E));
    }
    else
	{
		if constexpr (DoClassSerialize)
		{
			Serialize(value.GetClass().GetName());
		}

		if constexpr (DoBasicSerialize)
		{
			value.Serialize(*this);
		}

		if constexpr (DoDataSerialize)
		{
			if (value.ShouldSerializeData(*this))
			{
				value.SerializeData(*this);
			}
		}
	}
 
	return *this;
}

template <FreeListAllocatorGetter Allocator>
template<typename E>
Archive<Allocator>& Archive<Allocator>::Serialize(E** value)
{
    constexpr bool DumpBits = !IsSerializableObject<Allocator, E>;
    constexpr bool DoBasicSerialize = HasSerializeFunction<Allocator, E>;
    constexpr bool DoDataSerialize = HasSerializeDataFunction<Allocator, E>;

	// I am thinking of deprecating this feature completely.
	constexpr bool DoClassSerialize = false;// HasGetClassFunction<Allocator, E>;

	ED_ASSERT(value, "Pointer to a serialized pointer cannot be null");

    if constexpr (DumpBits)
    {
        if (GetMode() == SerializationMode::Read)
        {
            ED_ASSERT(value, "Can not dump bits for nullptr.");
            m_Source.Read(reinterpret_cast<void*>(*value), sizeof(E));
        }
        else
        {
			if (!value)
			{
				(*value) = Allocator().Allocate<E>();
			}
			m_Source.Write(reinterpret_cast<const void*>(*value), sizeof(E));
        }
    }
    else
	{
		ED_ASSERT(GetMode() == SerializationMode::Read || value, "Can not write nullptr.");

		//if constexpr (DoClassSerialize)
		//{
		//	if (GetMode() == SerializationMode::Write)
		//	{
		//		Serialize((*value)->GetClass().GetName());
		//	}
		//	else
		//	{
		//		std::string clazz;
		//		Serialize(clazz);
		//
		//		if (!(*value))
		//		{
		//			value = GameObjectFactory::CreateObject<E>(clazz);
		//		}
		//	}
		//}
		//else
		{
            if (GetMode() == SerializationMode::Read && !(*value))
            {
				(*value) = Allocator().Allocate<E>(1);
            }
		}

		if constexpr (DoBasicSerialize)
		{
			(*value)->Serialize(*this);
		}

		if constexpr (DoDataSerialize)
		{
			if ((*value)->ShouldSerializeData(*this))
			{
				(*value)->SerializeData(*this);
			}
		}
	}

	return *this;
}

template <FreeListAllocatorGetter Allocator>
template<typename E, bool IsRValue>
Archive<Allocator>& Archive<Allocator>::Serialize(E* values, u32 count, bool skipCount)
{
    constexpr bool DumpBits = !IsSerializableObject<Allocator, E>;
    constexpr bool DoBasicSerialize = HasSerializeFunction<Allocator, E>;
    constexpr bool DoDataSerialize = HasSerializeDataFunction<Allocator, E>;
    constexpr bool DoClassSerialize = HasGetClassFunction<Allocator, E>;

	if (!skipCount)
	{
		Serialize(count);
	}

    if constexpr (DumpBits)
    {
        if (GetMode() == SerializationMode::Read)
        {
            ED_ASSERT(!IsRValue, "Can not read to r value");

			if (values == nullptr)
			{
				values = Allocator().Allocate<E>(count);
			}

            m_Source.Read(reinterpret_cast<c8*>(values), sizeof(E) * count);
        }
        else
        {
            ED_ASSERT(values, "Can not write nullptr");

            m_Source.Write(reinterpret_cast<c8*>(values), sizeof(E) * count);
        }
    }
    else
    {
        ED_ASSERT(GetMode() == SerializationMode::Read || values, "Can not write nullptr.");

        if (GetMode() == SerializationMode::Read && values == nullptr)
        {
            values = Allocator().Allocate<E>(count);
        }

        for (u32 i = 0; i < count; ++i)
        {
            Serialize(values[i]);
        }
    }

    return *this;
}

template <FreeListAllocatorGetter Allocator>
template<typename E, bool IsRValue>
Archive<Allocator>& Archive<Allocator>::Serialize(E** values, u32 count)
{
	ED_ASSERT(GetMode() == SerializationMode::Read || values, "Can not write nullptr.");

	Serialize(count);

	if (GetMode() == SerializationMode::Read && (*values) == nullptr)
	{
		(*values) = Allocator().Allocate<E>(count);
	}

	Serialize<E, IsRValue>(*values, count, true);

	return *this;
}

template <FreeListAllocatorGetter Allocator>
template<typename E, bool IsRValue>
Archive<Allocator>& Archive<Allocator>::Serialize(std::shared_ptr<E>* values, u32 count)
{
    constexpr bool DumpBits = !IsSerializableObject<Allocator, E>;
    constexpr bool DoBasicSerialize = HasSerializeFunction<Allocator, E>;
    constexpr bool DoDataSerialize = HasSerializeDataFunction<Allocator, E>;
    constexpr bool DoClassSerialize = HasGetClassFunction<Allocator, E>;

    static_assert(!DumpBits, "Dump bits is not supported for pointer of pointers.");

    Serialize(count);

	ED_ASSERT(GetMode() == SerializationMode::Read || values, "Can not write nullptr.");
	ED_ASSERT(!IsRValue || GetMode() == SerializationMode::Write, "Can not write to r-value");

	if (GetMode() == SerializationMode::Read && values == nullptr)
    {
		values = new std::shared_ptr<E>[count];
	}

	for (u32 i = 0; i < count; ++i)
    {
		if (!values[i])
		{
			values[i] = std::make_shared<E>();
		}

		Serialize(*values[i]);
    }

    return *this;
}

template <FreeListAllocatorGetter Allocator>
template<typename E>
Archive<Allocator>& Archive<Allocator>::Serialize(std::vector<E>&& values)
{
    ED_ASSERT(GetMode() == SerializationMode::Write, "Can not read to r-value");

    Serialize(values.size());

    return Serialize<E, true>(values.data(), values.size());
}

template <FreeListAllocatorGetter Allocator>
template<typename E>
Archive<Allocator>& Archive<Allocator>::Serialize(std::vector<E>& values)
{
    u32 size = values.size();
    Serialize(size);

    if (GetMode() == SerializationMode::Read)
    {
        values.resize(size);
    }

    return Serialize<E, false>(values.data(), size);
}

template <FreeListAllocatorGetter Allocator>
Archive<Allocator>& Archive<Allocator>::Serialize(std::string& value)
{
    u32 count = (u32)value.size();
    Serialize(count);

    if (GetMode() == SerializationMode::Read)
    {
        value.resize(count);
    }

    return Serialize<c8, false>(value.data(), count, true);
}

template <FreeListAllocatorGetter Allocator>
Archive<Allocator>& Archive<Allocator>::Serialize(std::string&& value)
{
    ED_ASSERT(GetMode() == SerializationMode::Write, "Can not read to r-value");

    Serialize(value.size());

    return Serialize<c8, true>(value.data(), (u32)value.size(), true);
}