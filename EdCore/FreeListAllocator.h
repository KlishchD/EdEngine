#pragma once

// Free list allocator cannot be deallocated, so it is used as a base for all other storages
class FreeListAllocator
{
public:
    struct FreeListNode
    {
        static inline constexpr u32 Type = 0;
        static inline constexpr u64 SizeMask = (1ULL << 63) - 1;
        static inline constexpr u64 HeaderTypeMask = (1ULL << 63);
        
        __forceinline u32 GetType() const { return (m_Size & HeaderTypeMask) >> 63; }
        __forceinline u64 GetSize() const { return m_Size & SizeMask; }
        __forceinline FreeListNode* GetNext(u8* origin) { return m_Next > 0 ? reinterpret_cast<FreeListNode*>(origin + m_Next) : nullptr; }
        __forceinline FreeListNode* GetPrevious(u8* origin) { return m_Previous > 0 ? reinterpret_cast<FreeListNode*>(origin + m_Previous) : nullptr; }
        __forceinline i64 GetNext() { return m_Next; }
        __forceinline i64 GetPrevious() { return m_Previous; }

        __forceinline void Set(u64 size)
        {
            m_Size = size & SizeMask;
            m_Previous = -1;
            m_Next = -1;
        }

        __forceinline void Set(u64 size, u64 previous, u64 next)
        {
            m_Size = size & SizeMask;
            m_Previous = previous;
            m_Next = next;
        }

        __forceinline void Set(u64 size, u8* position, FreeListNode* previous, FreeListNode* next)
        {
            Set(size, position, reinterpret_cast<u8*>(previous), reinterpret_cast<u8*>(next));
        }

        __forceinline void Set(u64 size, u8* position, u8* previous, u8* next)
        {
            m_Size = size & SizeMask;
            SetPrevious(position, previous);
            SetNext(position, next);
        }

        __forceinline void SetNext(u8* origin, FreeListNode* node) { SetNext(origin, reinterpret_cast<u8*>(node)); }
        __forceinline void SetNext(u8* origin, u8* position) { m_Next = position - origin; }
        __forceinline void SetNext(u64 next) { m_Next = next; }
        __forceinline void SetPrevious(u8* origin, FreeListNode* node) { SetPrevious(origin, reinterpret_cast<u8*>(node)); }
        __forceinline void SetPrevious(u8* origin, u8* position) { m_Previous = position - origin; }
        __forceinline void SetPrevious(u64 previous) { m_Previous = previous; }

        __forceinline void Extend(u64 size) { m_Size += size; }
    protected:
        u64 m_Size;
        i64 m_Previous;
        i64 m_Next;
    };

    class FreeListAllocationHeader
    {
    public:
        static inline constexpr u32 Type = 1;
        static inline constexpr u64 SizeMask = (1ULL << 63) - 1;
        static inline constexpr u64 HeaderTypeMask = (1ULL << 63);

        __forceinline u32 GetType() const { return (m_Size & HeaderTypeMask) >> 63; }
        __forceinline u64 GetSize() const { return m_Size & SizeMask; }
        __forceinline u64 GetPrePadding() const { return m_PrePadding; }
        __forceinline u64 GetPostPadding() const { return m_PostPadding; }

        __forceinline u64 GetBlockSize() const { return m_PrePadding + m_Size + m_PostPadding; }

        __forceinline void Set(u64 size, u64 prePadding, u64 postPadding)
        {
            m_Size = size & SizeMask;
            m_Size = size | HeaderTypeMask;
            m_PrePadding = prePadding;
            m_PostPadding = postPadding;
        }
    protected:
        u64 m_Size;
        i64 m_PrePadding;
        i64 m_PostPadding;
    };

    enum class AllocationStrategy
    {
        FindFirst,
        FindBest
    };

    enum class AllocationSlotType
    {
        FullReplace,
        NewAllocation
    };

    FreeListAllocator(u64 size, const c8* debugName)
    {
        m_Data = Memory::Get().RequestStaticMemory<u8>(size, 1, debugName);

        m_ListHead = (FreeListNode*)m_Data;
        m_ListHead->Set(m_Size - sizeof(FreeListNode));

        m_Size = size;
    }

    template <typename T>
    T* Allocate(u64 count, u32 alignment = 8, AllocationStrategy strategy = AllocationStrategy::FindBest)
    {
        return reinterpret_cast<T*>(Allocate(sizeof(T) * count, alignment, strategy));
    }

    u8* Allocate(u64 size, u32 alignment = 8, AllocationStrategy strategy = AllocationStrategy::FindBest)
    {
        AllocationSlotType type;
        FreeListNode* node = nullptr;
        
        if (strategy == AllocationStrategy::FindBest)
        {
            node = FindBestFit(size, alignment, type);
        }
        else
        {
            node = FindFirstFit(size, alignment, type);
        }

        if (node == nullptr) return nullptr;

        u64 blockSize = node->GetSize();

        ED_ASSERT(node->GetType() == FreeListNode::Type, "Header type is not matching allocation header. Possible issues: attempted to allocate in incorrect block of memory, double deltion or header was corrupted.");
        ED_ASSERT(blockSize > size, "Reached capacity of the allocator.");

        if (type == AllocationSlotType::FullReplace)
        {
            u8* position = (u8*)node + sizeof(FreeListAllocationHeader);
            u8* dataPosition = Align(position, alignment);
            
            if (node == m_ListHead)
            {
                if (FreeListNode* next = node->GetNext(m_Data))
                {
                    m_ListHead = next;
                }
                else if (FreeListNode* previous = node->GetPrevious(m_Data))
                {
                    m_ListHead = previous;
                } else 
                {
                    m_ListHead = nullptr;
                }
            }

            FreeListAllocationHeader* header = (FreeListAllocationHeader*)(dataPosition - sizeof(FreeListAllocationHeader));
            u64 prePadding = dataPosition - position;
            u64 postPadding = blockSize - header->GetPrePadding() - size;
            header->Set(size, prePadding, postPadding);
            return dataPosition;
        }

        if (type == AllocationSlotType::NewAllocation)
        {
            u8* position = (u8*)node + sizeof(FreeListAllocationHeader);
            u8* dataPosition = Align(position, alignment);
            u64 prePadding = dataPosition - position;

            FreeListNode* newNode = (FreeListNode*)((u8*)dataPosition + size);
            u64 newNodeSize = node->GetSize() - (prePadding + size + sizeof(FreeListAllocationHeader));
            newNode->Set(newNodeSize, node->GetPrevious(), node->GetNext());

            if (FreeListNode* previous = node->GetPrevious(m_Data))
            {
                previous->SetNext(m_Data, newNode);
            }

            if (FreeListNode* next = node->GetNext(m_Data))
            {
                next->SetPrevious(m_Data, newNode);
            }

            if (node == m_ListHead)
            {
                m_ListHead = newNode;
            }

            FreeListAllocationHeader* header = (FreeListAllocationHeader*)(dataPosition - sizeof(FreeListAllocationHeader));
            header->Set(size, prePadding, 0);
            
            return dataPosition;
        }

        return nullptr;
    }

    template <typename T>
    void Deallocate(T* position, u64 count)
    {
        Deallocate(reinterpret_cast<u8*>(position), sizeof(T) * count);
    }

    void Deallocate(u8* position, u64 size)
    {
        ED_ASSERT(position >= m_Data || position < m_Data + m_Size, "Provided position is not owned by allocator.");
        ED_ASSERT(size, "Can not deallocate block of size 0.");
        ED_ASSERT(position, "Can not deallocate nullptr.");

        FreeListAllocationHeader* header = (FreeListAllocationHeader*)(position - sizeof(FreeListAllocationHeader));
        ED_ASSERT(size == header->GetSize(), "Some allocation data was corrupted or provided incorrect allocation position to deallocate.");
        ED_ASSERT(header->GetType() == FreeListAllocationHeader::Type, "Header type is not matching allocation header. Possible issues: attempted to deallocate incorrect block of memory, double deltion or header was corrupted.");

        u8* slotPosition = position - sizeof(FreeListAllocationHeader) - header->GetPrePadding();

        bool before;
        FreeListNode* previous = FindFirstNodeNeighbour(position, size, before);
        FreeListNode* next = previous ? previous->GetNext(m_Data) : nullptr;

        if (!before)
        {
            next = previous;
            previous = nullptr;
        }

        bool canConnectPrevious = false;
        bool canConnectNext = false;

        u64 blockSize = header->GetBlockSize();

        if (previous)
        {
            u8* previousEndPosition = (u8*)previous + previous->GetSize();
            canConnectPrevious = previousEndPosition >= slotPosition;
        }

        if (next)
        {
            u8* nextStartPosition = (u8*)next;
            canConnectNext = slotPosition + blockSize >= nextStartPosition;
        }

        if (canConnectPrevious && canConnectNext)
        {
            previous->SetNext(next->GetNext());
            
            if (FreeListNode* nextNext = next->GetNext(m_Data))
            {
                nextNext->SetPrevious(m_Data, previous);
            }

            previous->Extend(blockSize + sizeof(FreeListAllocationHeader));
            previous->Extend(next->GetSize() + sizeof(FreeListAllocator));
        }
        else if (canConnectPrevious)
        {
            previous->Extend(blockSize + sizeof(FreeListAllocationHeader));
        }
        else if (canConnectNext)
        {
            if (previous)
            {
                previous->SetNext(m_Data, slotPosition);
            }

            if (FreeListNode* nextNext = next->GetNext(m_Data))
            {
                nextNext->SetPrevious(m_Data, slotPosition);
            }

            FreeListNode* node = (FreeListNode*)slotPosition;
            node->Set(blockSize + (sizeof(FreeListNode) + next->GetSize()), next->GetPrevious(), next->GetNext());
        }
        else
        {
            FreeListNode* node = (FreeListNode*)slotPosition;
            node->Set(blockSize, m_Data, previous, next);
        
            if (previous)
            {
                previous->SetNext(m_Data, slotPosition);
            }

            if (next)
            {
                next->SetPrevious(m_Data, slotPosition);
            }
        }
    }

protected:
    FreeListNode* FindBestFit(u64 size, u32 alignment, AllocationSlotType& type)
    {
        FreeListNode* node = m_ListHead;

        FreeListNode* best = nullptr;
        u64 bestPadding = 10000;

        while (node)
        {
            u8* position = (u8*)node + sizeof(FreeListAllocationHeader);
            u8* alignedPosition = Align(position, alignment);

            u64 dataPadding = alignedPosition - position;
            u64 dataSize = size + dataPadding;

            u64 newAllocationSize = dataSize + sizeof(FreeListAllocationHeader);
            
            if (node->GetSize() >= newAllocationSize && bestPadding > dataPadding)
            {
                bestPadding = dataPadding;
                best = node;
                type = AllocationSlotType::NewAllocation;
            }

            u64 fullReplacePadding = node->GetSize() - size;
            if (node->GetSize() >= size && bestPadding >= fullReplacePadding)
            {
                bestPadding = fullReplacePadding;
                best = node;
                type = AllocationSlotType::FullReplace;
            }

            node = node->GetNext(m_Data);
        }

        return best;
    }

    FreeListNode* FindFirstFit(u64 size, u32 alignment, AllocationSlotType& type)
    {
        FreeListNode* node = m_ListHead;
        while (node)
        {
            u8* position = (u8*)node + sizeof(FreeListAllocationHeader);
            u8* alignedPosition = Align(position, alignment);
            u64 padding = alignedPosition - position;

            u64 allocationSize = size + padding;
            u64 fullSize = allocationSize + sizeof(FreeListAllocationHeader);
            if (node->GetSize() >= fullSize)
            {
                type = AllocationSlotType::NewAllocation;
                return node;
            }
            if (node->GetSize() >= allocationSize)
            {
                type = AllocationSlotType::FullReplace;
                return node;
            }

            node = node->GetNext(m_Data);
        }

        return nullptr;
    }

    FreeListNode* FindFirstNodeNeighbour(u8* position, u64 size, bool& before)
    {
        FreeListNode* previous = nullptr;
        FreeListNode* current = m_ListHead;
        while (current)
        {
            u8* currentPosition = (u8*)current;
            if (currentPosition > position)
            {
                if (previous)
                {
                    before = true;
                    return previous;
                }
                
                before = false;
                return current;
            }

            previous = current;
            current = current->GetNext(m_Data);
        }

        before = false;
        return nullptr;
    }

protected:
    u8* m_Data;
    FreeListNode* m_ListHead;
    u64 m_Size;
};