#include "EdCore.h"
#include "Memory.h"

Memory::Memory()
{
    m_Heap = (u8*)malloc(HeapSize);

    m_StaticDataPointer = m_Heap + HeapSize;
    m_DynamicDataPointer = m_Heap;
}

void Memory::OnNewFrame()
{
    for (u32 i = 0; i < m_DeallocationRequests.size();)
    {
        DeallocationRequest& request = m_DeallocationRequests[i];
        
        if (request.Predicate())
        {
            u8* ptr = *request.Data;
            request.Data = nullptr;

            request.Allocator->Deallocate(ptr, request.Size);
            m_DeallocationRequests.erase(m_DeallocationRequests.begin() + i);
        
            continue;
        }

        ++i;
    }

    m_DynamicDataPointer = m_Heap;
#if defined(DEBUG_BUILD) || defined(DEVELOPMENT_BUILD)
    m_DynamicAllocations.clear();
#endif
}

void* Memory::RequestStaticMemory(u64 bytes, u32 alignment, ccstr8 debugName)
{
    if (bytes == 0) return nullptr;

    m_StaticDataPointer = Align(m_StaticDataPointer - bytes - alignment, alignment);

    ED_ASSERT(m_DynamicDataPointer < m_StaticDataPointer, "Heap size was exceeded.");

#if defined(DEBUG_BUILD) || defined(DEVELOPMENT_BUILD)
    m_StaticAllocations.push_back(std::make_pair(bytes, debugName));
#endif

    return reinterpret_cast<void*>(m_StaticDataPointer);
}

void* Memory::RequestDynamicMemory(u64 bytes, u32 alignment, ccstr8 debugName)
{
    if (bytes == 0) return nullptr;

    u8* slot = m_DynamicDataPointer;
    
    m_DynamicDataPointer = Align(m_DynamicDataPointer + bytes, alignment);

    ED_ASSERT(m_DynamicDataPointer < m_StaticDataPointer, "Heap size was exceeded.");

#if defined(DEBUG_BUILD) || defined(DEVELOPMENT_BUILD)
    m_DynamicAllocations.push_back(std::make_pair(reinterpret_cast<u64>(m_DynamicDataPointer), debugName));
#endif

    return reinterpret_cast<void*>(slot);
}

void* Memory::RequestMemory(u32 size, bool temporary, bool clear)
{
    void* buffer;

    if (temporary)
    {
        buffer = RequestDynamicMemory(size + 1, 1, "TemporaryBuffer");
    }
    else
    {
        buffer = RequestStaticMemory<c8>(size + 1, 1, "Buffer");
    }

    if (clear)
    {
        memset(buffer, 0, size);
    }

    return buffer;
}

void* Memory::RequestMemory(const void* buffer, u32 bufferSize, u32 size, bool temporary, bool clear)
{
    ED_ASSERT(bufferSize <= size, "Destnation buffer can not be bigger than origin buffer.");

    void* result = RequestMemory(size, temporary, clear);

    memcpy(result, buffer, size);

    return result;
}

void Memory::AddDeallocation(FreeListAllocator* allocator, void** data, u32 size, const MemoryDeallocationPredicate& predicate)
{
    DeallocationRequest request;
    request.Allocator = allocator;
    request.Data = reinterpret_cast<u8**>(data);
    request.Size = size;
    request.Predicate = predicate;
    m_DeallocationRequests.push_back(request);
}

Memory::~Memory()
{
    free(m_Heap);
}

