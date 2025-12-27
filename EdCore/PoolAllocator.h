#pragma once

// This is a quick implementation and I guess could be improved upon
template <typename ObjectType, u32 MaxCount>
class PoolAllocator
{
protected:
    static constexpr u32 InvalidSlot = (1 << 31);

protected:
    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = ObjectType;
        using pointer = value_type*;
        using reference = value_type*;

        Iterator(ObjectType* data, const BitMask32& mask, u32 index, u32 firstFreeSlot, u32 firstUnusedSlot) : m_Data(data), m_Mask(mask), m_Index(index), m_NextFreeSloot(firstFreeSlot), m_FirstUnusedSlot(firstUnusedSlot)
        {
            while (m_Index < m_FirstUnusedSlot && !m_Mask.Get(m_Index)) ++m_Index;
        }

        reference operator*() const { return m_Index >= m_FirstUnusedSlot ? nullptr : m_Data + m_Index; }
        pointer operator->() { return m_Index >= m_FirstUnusedSlot ? nullptr : m_Data + m_Index; }

        Iterator& operator++() {
            do {
                ++m_Index;
            } while (m_Index < m_FirstUnusedSlot && !m_Mask.Get(m_Index));

            return *this;
        }

        Iterator operator++(i32) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_Index == b.m_Index || (a.IsPointingOutside() && b.IsPointingOutside()); };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_Index != b.m_Index; };

    protected:
        inline bool IsPointingOutside() const { return m_Index >= m_FirstUnusedSlot; }
    protected:
        ObjectType* m_Data;
        const BitMask32& m_Mask; // TODO: Make a separate structure out of this field
        u32 m_Index;
        u32 m_NextFreeSloot;
        u32 m_FirstUnusedSlot;
    };
public:
    PoolAllocator(const c8* debugName) : m_Mask(MaxCount), m_FirstFreeSlot(InvalidSlot), m_FirstUnusedSlot(0), m_AllocatedCount(0)
    {
        static_assert(sizeof(ObjectType) >= sizeof(u32), "Object should at least 4 bytes to be used with this implementation of pool allocator");
        m_Data = Memory::Get().RequestStaticMemory<ObjectType>(MaxCount, 8, debugName);
    }

    ObjectType* Allocate()
    {
        ED_ASSERT(m_FirstFreeSlot != InvalidSlot || m_FirstUnusedSlot < MaxCount, "Exceeded the capacity of the pool");

        void* pointer;

        if (m_FirstFreeSlot != InvalidSlot)
        {
            pointer = reinterpret_cast<void*>(m_Data + m_FirstFreeSlot);
            m_FirstFreeSlot = *reinterpret_cast<u32*>(pointer);
        }
        else
        {
            pointer = reinterpret_cast<void*>(m_Data + m_FirstUnusedSlot);
            ++m_FirstUnusedSlot;
        }

        ++m_AllocatedCount;
        m_Mask.Set(u32(reinterpret_cast<ObjectType*>(pointer) - m_Data), true);

        return new (pointer) ObjectType();
    }

    void Deallocate(ObjectType* pointer)
    {
        ED_ASSERT(pointer >= m_Data && pointer <= m_Data + MaxCount, "Can not deallocate data that does not belong to this allocator.");
        ED_ASSERT(m_Mask.Get(u32(reinterpret_cast<ObjectType*>(pointer) - m_Data)), "Can not deallocate data that was no allocated by this allocator.");

        pointer->~ObjectType();

        u32* val = reinterpret_cast<u32>(pointer);
        u32* slot = m_FirstFreeSlot != 0 ? m_Data + m_FirstFreeSlot : val;

        if (val >= slot)
        {
            (*val) = m_FirstFreeSlot;
            m_FirstFreeSlot = pointer - m_Data;
        }
        else
        {
            u32 nextFree = *slot;

            (*val) = nextFree;
            (*slot) = pointer - m_Data;
        }

        m_Mask.Set(u32(reinterpret_cast<ObjectType*>(pointer) - m_Data), false);

        --m_AllocatedCount;
    }

    void Clear()
    {
        for (ObjectType* object : (*this))
        {
            object->~ObjectType();
        }

        m_Mask.Clear();
        m_FirstFreeSlot = InvalidSlot;
        m_FirstUnusedSlot = 0;
        m_AllocatedCount = 0;
    }

    u32 GetAllocatedObjectsCount() const { return m_AllocatedCount; }

    Iterator begin() { return Iterator(m_Data, m_Mask, 0, m_FirstFreeSlot, m_FirstUnusedSlot); }
    Iterator end() { return Iterator(m_Data, m_Mask, m_AllocatedCount, m_FirstFreeSlot, m_FirstUnusedSlot); }

    Iterator begin() const { return Iterator(m_Data, m_Mask, 0, m_FirstFreeSlot, m_FirstUnusedSlot); }
    Iterator end() const { return Iterator(m_Data, m_Mask, m_AllocatedCount, m_FirstFreeSlot, m_FirstUnusedSlot); }
protected:
    ObjectType* m_Data;
    BitMask32 m_Mask;
    u32 m_FirstFreeSlot;
    u32 m_FirstUnusedSlot;
    u32 m_AllocatedCount;
};