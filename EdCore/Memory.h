#pragma once

class FreeListAllocator;

using MemoryDeallocationPredicate = std::function<bool()>;

using FreeListAllocatorGetter = FreeListAllocator&(*)();

#define FREE_LIST_ALLOCATOR(name, size) \
    inline FreeListAllocator& name() \
    { \
        static FreeListAllocator _ ## name(static_cast<u64>(size), #name); \
        return _ ## name; \
    }

// Note:
// Static data is used only for predefined arrays and some static structures that are not being deallocated.
// Dynamic data is used only for data that is needed only for one frame and will be cleared in the begging of the next one.
// Memory class is a Double-Ended stack allocator.
class Memory
{
public:
    static constexpr u64 BytesToKB = 1024;
    static constexpr u64 BytesToMB = 1024 * BytesToKB;
    static constexpr u64 BytesToGB = 1024 * BytesToMB;
    static constexpr u64 HeapSize = 8 * BytesToGB;
public:
    static inline Memory& Get()
    {
        static Memory memory;
        return memory;
    }

    void OnNewFrame();

    // Static memory is static and is never deallocated, so no method for that
    void* RequestStaticMemory(u64 bytes, u32 alignment, ccstr8 debugName);
    template <typename T>
    T* RequestStaticMemory(u64 count, u32 alignment, ccstr8 debugName)
    {
        return reinterpret_cast<T*>(RequestStaticMemory(sizeof(T) * count, alignment, debugName));
    }

    template <typename T, typename... Types>
    T* CreateStatic(Types&&... types)
    {
        void* memory = RequestStaticMemory<T>(1, nullptr);
        return new (memory) T(std::forward<Types>(types)...);
    }

    void* RequestDynamicMemory(u64 bytes, u32 alignment, ccstr8 debugName);
    template <typename T>
    T* RequestDynamicMemory(u64 count, u32 alignment, ccstr8 debugName)
    {
        return reinterpret_cast<T*>(RequestDynamicMemory(sizeof(T) * count, alignment, debugName));
    }

    void* RequestMemory(u32 size, bool temporary, bool clear);
    void* RequestMemory(const void* buffer, u32 bufferSize, u32 size, bool temporary, bool clear);

    template <typename T>
    T* RequestMemory(u32 count, bool temporary, bool clear)
    {
        return reinterpret_cast<T*>(RequestMemory(count * sizeof(T), temporary, clear));
    }

    template <typename T>
    T* RequestMemory(const void* buffer, u32 bufferSize, u32 count, bool temporary, bool clear)
    {
        return reinterpret_cast<T*>(RequestMemory(buffer, bufferSize, count * sizeof(T), temporary, clear));
    }

    void AddDeallocation(FreeListAllocator* allocator, void** data, u32 size, const MemoryDeallocationPredicate& predicate);

    template <typename T>
    void AddDeallocation(FreeListAllocator* allocator, T** data, u32 count, const MemoryDeallocationPredicate& predicate)
    {
        AddDeallocation(allocator, reinterpret_cast<void**>(data), sizeof(T) * count, predicate);
    }
protected:
    Memory();
    ~Memory();

    friend class Engine;

protected:
    u8* m_Heap;

    u8* m_StaticDataPointer;
    u8* m_DynamicDataPointer;

    // I don't like STL being used here but it will take a lot of time to remove it now
    // because of somewhat shitty memory dependencies, so it will be cleaned in a dedicated task.
    // https://klishchdmytro.atlassian.net/browse/OPS-60
    // But overall memory system does what I wanted and almost how I wanted it ;), so I consider 
    // this funny experiment a success)

#if defined(DEBUG_BUILD) || defined(DEVELOPMENT_BUILD)
    std::vector<std::pair<u64, ccstr8>> m_StaticAllocations;
    std::vector<std::pair<u64, ccstr8>> m_DynamicAllocations;
#endif

    struct DeallocationRequest
    {
        FreeListAllocator* Allocator;
        u8** Data;
        u32 Size;
        MemoryDeallocationPredicate Predicate;
    };

    std::vector<DeallocationRequest> m_DeallocationRequests;
};

inline u8* Align(u8* pointer, u64 alignment)
{
    return reinterpret_cast<u8*>((reinterpret_cast<u64>(pointer) + alignment - 1ULL) & ~(alignment - 1ULL));
}

inline u64 Align(u64 pointer, u64 alignment)
{
    return (pointer + alignment - 1ULL) & ~(alignment - 1ULL);
}

inline u32 Tile(u32 value, u32 size)
{
    return (value + size - 1) / size;
}

template <typename buffer_type, typename value_type>
inline buffer_type* PutValue(buffer_type* buffer, const value_type* value, u32 count)
{
  const u32 size = sizeof(value_type) * count;
  memcpy(buffer, value, size);
  return reinterpret_cast<buffer_type*>(reinterpret_cast<u8*>(buffer) + size);
}

template <typename buffer_type, typename value_type>
inline buffer_type* PutValue(buffer_type* buffer, const value_type& value)
{
  memcpy(buffer, &value, sizeof(value_type));
  return reinterpret_cast<buffer_type*>(reinterpret_cast<u8*>(buffer) + sizeof(value_type));
}

template <typename buffer_type, typename value_type = u8>
inline buffer_type* OffsetBuffer(buffer_type* buffer, u32 count)
{
  return reinterpret_cast<buffer_type*>(reinterpret_cast<u8*>(buffer) + sizeof(value_type) * count);
}