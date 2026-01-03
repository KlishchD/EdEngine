#pragma once

class Heap
{
public:
    Heap(HeapType type, HeapFlags flags, u64 size, u32 alignment, bool resident, ccstr8 name = "DefaultHeapName", i32 nameSize = -1);

    void SetDebugName(ccstr8 name, i32 size = -1);
    void GetDebugName(ccstr8& name, i32& size);

    // Here I would like for it to decide by it selft whether it give me right next spot
    // or aligned next spot or smth else, I would like to give it configuration and it 
    // should do everything itself.
    Resource* CreateResource(ResourceFlags flags, ResourceState sate, PixelFormat format, u32 width, u32 height, u32 depth, u32 mips, ccstr8 name = "DefaultResourceName", i32 nameSize = -1);
    Resource* CreateResource(ResourceFlags flags, ResourceState sate, PixelFormat format, u32 width, u32 height, u32 mips, ccstr8 name = "DefaultResourceName", i32 nameSize = -1);
    Resource* CreateResource(ResourceFlags flags, ResourceState sate, PixelFormat format, u32 width, u32 height, ccstr8 name = "DefaultResourceName", i32 nameSize = -1);
    Resource* CreateResource(ResourceFlags flags, ResourceState sate, u32 count, u64 itemSize, ccstr8 name = "DefaultResourceName", i32 nameSize = -1);

    void DeleteResource(Resource* resource);

    template <typename T>
    __forceinline T* GetNativeHandle() const
    {
        return (T*)m_NativeHandle;
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    ~Heap();
protected:
    uptr m_NativeHandle;

    Array<Resource*> m_Resources;

    u64 m_Size;
    u64 m_Offset;
    u32 m_Alignment;
    bool m_Resident;
};