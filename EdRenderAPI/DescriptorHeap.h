#pragma once

class DescriptorHeap
{
public:
    DescriptorHeap(DescriptorHeapType type, u32 count, bool shaderVisible, ccstr8 name = "DefaultDescriptorHeapName", i32 nameSize = -1);

    void SetDebugName(ccstr8 name, i32 size = -1);
    void GetDebugName(ccstr8& name, i32& size);

    ResourceView CreateView(DescriptorHeapType type, Resource* resource, u32 mip);
    ResourceView CreateView(DescriptorHeapType type, Resource* resource);
    void FreeView(ResourceView view);

    u64 GetStartLocation() const;
    u64 GetDescriptorSize() const;
    u32 GetDescriptorIndex(u64 location) const;
    u32 GetDescriptorIndex(const ResourceView& view) const;

    void Reserve(u32 slot);
    u32 ReserveSlot();

    template <typename T>
    T* GetNativeHandle() const
    {
        return reinterpret_cast<T*>(m_NativeHandle);
    }

    __forceinline uptr GetNativeHandle() const { m_NativeHandle; }

    ~DescriptorHeap();
protected:
    uptr m_NativeHandle;
    u64 m_StartLocation;
    u64 m_DescriptorSize;
    BitMask32 m_Mask;
    DescriptorHeapType m_Type;
    u32 m_Count;
    bool m_ShaderVisible;
};
