#include "EdDirectX12.h"

#define GetResource() reinterpret_cast<ID3D12Resource*>(m_NativeHandle)

Resource::Resource(ResourceFlags flags, ResourceState state, uptr handle, PixelFormat format, u32 width, u32 height, u32 depth, u32 mips, ccstr8 name) : m_NativeHandle(handle), m_Type(TextureType), m_Format(format), m_Width(width), m_Height(height), m_Depth(depth), m_Subresources(mips), m_Stride(1), m_Flags(flags)
{
    for (u32 mip = 0; mip < mips; ++mip)
    {
        m_States.Add(state);
    }

    SetDebugName(name);
}

Resource::Resource(ResourceFlags flags, ResourceState state, uptr handle, u32 count, u64 itemSize, ccstr8 name) : m_NativeHandle(handle), m_Type(BufferType), m_Format(PixelFormat::R8F), m_Width(count), m_Height(1), m_Depth(1), m_Subresources(1), m_Stride(itemSize), m_Flags(flags)
{
    m_States.Add(state);

    SetDebugName(name);
}

NAME_METHODS_TEMPLATE(Resource, ID3D12Resource)

bool Resource::Map(u32 subresource, u64 begin, u64 end, void** data)
{
    D3D12_RANGE range;
    range.Begin = begin;
    range.End = end;
    return GetResource()->Map(subresource, &range, data) == S_OK;
}

void Resource::Unmap(u32 subresource, u64 begin, u64 end)
{
    D3D12_RANGE range;
    range.Begin = begin;
    range.End = end;
    GetResource()->Unmap(subresource, &range);
}

u64 Resource::GetGpuAddress() const
{
    return GetResource()->GetGPUVirtualAddress();
}

void Resource::MakeResident()
{
    ID3D12Pageable* resource = GetResource();
    D3D::Check(g_Device->MakeResident(1, &resource));
}

void Resource::Evict()
{
    ID3D12Pageable* resource = GetResource();
    D3D::Check(g_Device->Evict(1, &resource));
}

ResourceState Resource::GetState(CommandListType type, u32 subresoruce) const
{
    ED_ASSERT(subresoruce < m_Subresources, "Atempted to get state of non exising subresoruce.");
    return m_States[subresoruce];
}

void Resource::SetState(CommandListType type, u32 subresoruce, ResourceState state)
{
    ED_ASSERT(subresoruce < m_Subresources, "Atempted to set state of non exising subresoruce.");
    m_States[subresoruce] = state;
}

Resource::~Resource()
{
    const bool isHandleOwner = !(m_Flags & RF_HolderResource);
    if (isHandleOwner)
    {
        GetNativeHandle<ID3D12Resource>()->Release();
    }

    m_NativeHandle = 0;
}
