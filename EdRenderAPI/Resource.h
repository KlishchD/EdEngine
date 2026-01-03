#pragma once

class Resource
{
public:
    enum Type
    {
        BufferType,
        TextureType
    };

    Resource(ResourceFlags flags, ResourceState state, uptr handle, PixelFormat format, u32 width, u32 height, u32 depth, u32 mips, ccstr8 name = "DefaultResourceName", i32 nameSize = -1);
    Resource(ResourceFlags flags, ResourceState state, uptr handle, u32 count, u64 itemSize, ccstr8 name = "DefaultResourceName", i32 nameSize = -1);

    void SetDebugName(ccstr8 name, i32 size = -1);
    void GetDebugName(ccstr8& name, i32& size);

    bool Map(u32 subresource, u64 begin, u64 end, void** data);
    void Unmap(u32 subresource, u64 begin, u64 end);

    PixelFormat GetPixelFormat() const { return m_Format; }
    u32 GetWidth() const { return m_Width; }
    u32 GetHeight() const { return m_Height; }
    u32 GetDepth() const { return m_Depth; }
    Type GetType() const { return m_Type; }
    u32 GetSubresourcesCount() const { return m_Subresources; }

    u32 GetSize() const { return m_Width * m_Height * m_Depth * m_Stride * RenderTypes::GetPixelSize(m_Format); }
    u32 GetStride() const { return m_Stride; }

    void MakeResident();
    void Evict();

    ResourceState GetState(CommandListType type, u32 subresoruce) const;
    void SetState(CommandListType type, u32 subresoruce, ResourceState state);

    u64 GetGpuAddress() const;

    template <typename T>
    T* GetNativeHandle() const
    {
        return reinterpret_cast<T*>(m_NativeHandle);
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    ~Resource();
protected:
    uptr m_NativeHandle;
    Array<ResourceState> m_States;
    Type m_Type;
    PixelFormat m_Format;
    u32 m_Width;
    u32 m_Height;
    u32 m_Depth;
    u32 m_Subresources;
    u32 m_Stride;
    ResourceFlags m_Flags;
};

class Subresource
{
public:
    Subresource(Resource* resoruce, u32 offset, u32 width, u32 height, u32 depth) : m_Resource(resoruce), m_Offset(offset), m_Width(width), m_Height(height), m_Depth(depth)
    {

    }

    u32 GetOffset() const { return m_Offset; }
    u32 GetWidth() const { return m_Width; }
    u32 GetHeight() const { return m_Height; }
    u32 GetDepth() const { return m_Depth; }

    PixelFormat GetPixelFormat() const { return m_Resource->GetPixelFormat(); }
    Resource::Type GetType() const { return m_Resource->GetType(); }

    template <typename T>
    __forceinline T* GetNativeHandle() const { return m_Resource->GetNativeHandle<T>(); }
    __forceinline uptr GetNativeHandle() const { return m_Resource->GetNativeHandle(); }
protected:
    Resource* m_Resource;
    ResourceState m_State;
    u32 m_Offset;
    u32 m_Width;
    u32 m_Height;
    u32 m_Depth;
};