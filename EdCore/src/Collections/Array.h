#pragma once

template <typename T, bool Static = false, bool Temporary = false, FreeListAllocatorGetter Allocator = &DataStructuresAllocator>
class Array
{
public:
    Array(u32 capacity = 10) : m_Capacity(std::max<u32>(1, capacity)), m_Size(0)
    {
        static_assert(Allocator, "Allocator can not be nullptr.");

        if constexpr (Temporary)
        {
            m_Data = Memory::Get().RequestDynamicMemory<T>(m_Capacity, 8, "TemporaryArray");
        }
        else
        {
            m_Data = Allocator().Allocate<T>(m_Capacity, 8);
        }
    }

    template <bool OtherStatic, bool OtherTemporary, FreeListAllocatorGetter OtherAllocator>
    Array(const Array<T, OtherStatic, OtherTemporary, OtherAllocator>& array)
    {
        static_assert(Static == OtherStatic && Temporary == OtherTemporary && Allocator == OtherAllocator, "Collection type mixing is not yet supported");

        MoveAndReallocate<false>(array.m_Size);

        memcpy(m_Data, array.m_Data, sizeof(T) * array.m_Size);

        m_Size = array.m_Size;
    }

    template <bool OtherStatic, bool OtherTemporary, FreeListAllocatorGetter OtherAllocator>
    Array(Array<T, OtherStatic, OtherTemporary, OtherAllocator>&& array) : m_Data(array.m_Data), m_Size(array.m_Size), m_Capacity(array.m_Capacity)
    {
        static_assert(Static == OtherStatic && Temporary == OtherTemporary && Allocator == OtherAllocator, "Collection type mixing is not yet supported");
        static_assert(Allocator == OtherAllocator, "Can not move pointer to different allocator as it has no ownership over it.");

        array.m_Data = nullptr;
        array.m_Size = 0;
        array.m_Capacity = 0;
    }

    T& Add()
    {
        if (m_Size >= m_Capacity)
        {
            ED_ASSERT(!Static, "Exceeded static array capacity")

            MoveAndReallocate<true>();
        }

        T* object = new (m_Data + m_Size) T ();
        
        ++m_Size;

        return *object;
    }

    void Add(const T& item)
    {
        Add() = item;
    }

    void Add(T&& item)
    {
        Add() = std::move(item);
    }

    bool AddUnique(const T& item)
    {
        for (u32 i = 0; i < m_Size; ++i)
        {
            if (m_Data[i] == item)
            {
                return false;
            }
        }

        Add() = item;

        return true;
    }

    bool AddUnique(T&& item)
    {
        for (u32 i = 0; i < m_Size; ++i)
        {
            if (m_Data[i] == item)
            {
                return false;
            }
        }

        Add() = std::move(item);

        return true;
    }

    void Remove(u32 index)
    {
        if (index >= m_Size) return;

        for (u32 i = index; i + 1 < m_Size; ++i)
        {
            memcpy(m_Data + i, m_Data + i + 1, sizeof(T));
        }

        m_Size--;
    }

    bool Remove(const T& item)
    {
        bool bFound = false;

        for (u32 i = 0; i < m_Size; ++i)
        {
            if (!bFound && m_Data[i] == item)
            {
                bFound = true;
            }

            if (bFound)
            {
                memcpy(m_Data + i, m_Data + i + 1, sizeof(T));
            }
        }

        m_Size -= (u32)bFound;

        return bFound;
    }

    void SwapRemove(u32 index)
    {
        if (index >= m_Size) return;

        if (index != m_Size - 1)
        {
            memcpy(m_Data + index, m_Data + m_Size - 1, sizeof(T));
        }

        --m_Size;
    }

    bool SwapRemove(const T& item)
    {
        bool bFound = false;

        for (u32 i = 0; i < m_Size; ++i)
        {
            if (m_Data[i] == item)
            {
                bFound = true;

                if (i != m_Size)
                {
                    memcpy(m_Data + i, m_Data + m_Size - 1, sizeof(T));
                }
            }
        }

        m_Size -= (u32)bFound;

        return bFound;
    }

    u32 RemoveAll(const T& item)
    {
        u32 count = 0;

        u32 curr = 0;
        u32 next = 0;

        while (next < m_Size)
        {
            if (m_Data[next] == item)
            {
                ++count;
                ++next;
                continue;
            }

            if (curr != next)
            {
                memcpy(m_Data + curr, m_Data + next, sizeof(T));
            }

            ++curr;
            ++next;
        }

        m_Size -= (u32)count;

        return count;
    }

    u32 SwapRemoveAll(const T& item)
    {
        u32 count = 0;
        u32 left = 0;
        u32 right = m_Size - 1;
        
        while (right > left)
        {
            if (m_Data[right] == item)
            {
                --right;
                ++count;
                continue;
            }

            if (m_Data[left] == item)
            {
                memcpy(m_Data + left, m_Data + right, sizeof(T));
                ++left;
                --right;
                ++count;
                continue;
            }

            ++left;
        }

        return count;
    }

    void RemoveLast()
    { 
        ED_ASSERT(m_Size, "Can not remove last from an empty array.");
        Remove(m_Size - 1);
    }
    void RemoveFirst()
    {
        ED_ASSERT(m_Size, "Can not remove first from an empty array.");
        Remove(0U);
    }

    T GetAndRemoveLast()
    {
        ED_ASSERT(m_Size, "Can not get/remove last from an empty array.");

        T object = m_Data[m_Size - 1];
        Remove(m_Size - 1);
        return object;
    }

    T GetAndRemoveFirst()
    {
        ED_ASSERT(m_Size, "Can not get/remove first from an empty array.");

        T object = m_Data[0];
        Remove(0U);
        return object;
    }

    T GetAndSwapRemoveFirst()
    {
        ED_ASSERT(m_Size, "Can not get/remove first from an empty array.");

        T object = m_Data[0];
        SwapRemove(0U);
        return object;
    }

    i32 Find(const T& item)
    {
        for (i32 i = 0; i < m_Size; ++i)
        {
            if (m_Data[i] == item)
            {
                return i;
            }
        }

        return -1;
    }

    void Clear()
    {
        for (u32 i = 0; i < m_Size; ++i)
        {
            m_Data[i].~T();
        }

        m_Size = 0;
    }

    const T& GetFirst() const { ED_ASSERT(m_Size, "Can not get fisrt of an empty array."); return m_Data[0]; }
    const T& GetLast() const { ED_ASSERT(m_Size, "Can not get last of an empty array."); return m_Data[m_Size - 1]; }

    bool IsEmpty() const { return m_Size == 0; }
    u32 GetSize() const { return m_Size; }
    u32 GetCapacity() const { return m_Capacity; }

    T* Get() const { return m_Data; }

    void Reserve(u32 capacity)
    {
        if (capacity > m_Capacity)
        {
            MoveAndReallocate<true>(capacity);
        }
    }

    const T& operator[] (u32 index) const
    {
        ED_ASSERT(index < m_Size, "Array size exceeded.");
        return m_Data[index];
    }

    T& operator[] (u32 index)
    {
        ED_ASSERT(index < m_Size, "Array size exceeded.");
        return m_Data[index];
    }

    template <bool OtherStatic, bool OtherTemporary, FreeListAllocatorGetter OtherAllocator>
    Array<T, Static, Temporary>& operator=(const Array<T, OtherStatic, OtherTemporary, OtherAllocator>& array)
    {
        static_assert(Static == OtherStatic && Temporary == OtherTemporary, "Collection type mixing is not yet supported");

        if (array.m_Size > m_Capacity)
        {
            MoveAndReallocate<false>(array.GetSize());
        }

        memcpy(m_Data, array.m_Data, sizeof(T) * array.m_Size);
        m_Size = array.m_Size;

        return *this;
    }

    template <bool OtherStatic, bool OtherTemporary, FreeListAllocatorGetter OtherAllocator>
    Array<T, Static, Temporary>& operator=(Array<T, OtherStatic, OtherTemporary, OtherAllocator>&& array)
    {
        static_assert(Allocator == OtherAllocator, "Can not move pointer to different allocator as it has no ownership over it.");
        static_assert(Static == OtherStatic && Temporary == OtherTemporary, "Collection type mixing is not yet supported");

        Free();

        m_Size = array.m_Size;
        m_Capacity = array.m_Capacity;
        m_Data = array.m_Data;

        array.m_Size = 0;
        array.m_Capacity = 0;
        array.m_Data = nullptr;

        return *this;
    }

    ~Array()
    {
        Free();
    }

public:
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;

        Iterator(T* data) : m_Data(data)
        {

        }

        reference operator*() { ED_ASSERT(m_Data, "Can not dereference iterator pointing to nullptr"); return *m_Data; }
        pointer operator->() { ED_ASSERT(m_Data, "Can not dereference iterator pointing to nullptr"); return m_Data; }

        Iterator& operator++() 
        { 
            ++m_Data;
            return *this;
        }

        Iterator& operator++(int) 
        {
            Iterator iterator = this; 
            ++m_Data; 
            return iterator;
        }

        bool operator==(const Iterator& other) const { return m_Data == other.m_Data; }
        bool operator!=(const Iterator& other) const { return m_Data != other.m_Data; }

    protected:
        T* m_Data;
    };

    Iterator begin() const { return Iterator(m_Data); }
    Iterator end() const { return Iterator(m_Data + m_Size); }
protected:
    void Free()
    {
        if (m_Capacity <= 0)
        {
            return;
        }

        for (u32 i = 0; i < m_Size; ++i)
        {
            m_Data[i].~T();
        }

        if constexpr (!Temporary)
        {
            Allocator().Deallocate<T>(m_Data, m_Capacity);
        }

        m_Size = 0;
        m_Capacity = 0;
        m_Data = nullptr;
    }

    template <bool copy>
    void MoveAndReallocate()
    {
        MoveAndReallocate<copy>((u32)(m_Capacity * 1.5));
    }

    template <bool copy>
    void MoveAndReallocate(u32 capacity)
    {
        if (capacity <= m_Capacity) {
            return;
        }

        T* data;

        if constexpr (Temporary)
        {
            data = Memory::Get().RequestDynamicMemory<T>(capacity, 8, "TemporaryArray");
        }
        else
        {
            data = Allocator().Allocate<T>(capacity, 8);
        }

        if constexpr (copy)
        {
            if (m_Data)
            {
                memcpy(data, m_Data, m_Size * sizeof(T));
            }
        }

        if constexpr (!Temporary)
        {
            Allocator().Deallocate<T>(m_Data, m_Capacity);
        }

        m_Capacity = capacity;
        m_Data = data;
    }

protected:
    T* m_Data;
    u32 m_Capacity;
    u32 m_Size;
};

template <typename T, u32 Size>
class StaticArray : public Array<T, true, false, &DataStructuresAllocator>
{
    using Super = Array<T, true, false, &DataStructuresAllocator>;
public:
    StaticArray() : Super(Size)
    {
        Super::m_Size = Size;
        new (Super::m_Data) T[Size];
    }
};

template <typename T>
using StaticTemporaryArray = Array<T, true, true>;

template <typename T>
using TemporaryArray = Array<T, false, true>;
