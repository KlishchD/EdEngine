#include "EdDirectX12.h"

#define GetHeap() GetNativeHandle<ID3D12Heap>()

Heap::Heap(HeapType type, HeapFlags flags, u64 size, u32 alignment, bool resident, ccstr8 name, i32 nameSize) : m_Size(size), m_Offset(0), m_Alignment(alignment), m_Resident(resident)
{
    D3D12_HEAP_DESC description;
    description.SizeInBytes = size;
    description.Properties.Type = resident ? D3D12_HEAP_TYPE_CUSTOM : DirectX12Types::ConvertHeapType(type);
    description.Properties.CPUPageProperty = resident ? D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE : D3D12_CPU_PAGE_PROPERTY_UNKNOWN;// DirectX12Types::ConvertCPUPageProperty(type);
    description.Properties.MemoryPoolPreference = resident ? D3D12_MEMORY_POOL_L1 : D3D12_MEMORY_POOL_UNKNOWN;// DirectX12Types::ConvertMemoryPool(type);
    description.Properties.CreationNodeMask = 0;
    description.Properties.VisibleNodeMask = 0;
    description.Alignment = alignment;
    description.Flags = DirectX12Types::ConvertHeapFlags(flags);

    D3D::Check(g_Device->CreateHeap(&description, __uuidof(ID3D12Heap), reinterpret_cast<void**>(&m_NativeHandle)));

    SetDebugName(name, nameSize);

    if (resident)
    {
        ID3D12Pageable* heap = GetHeap();
        D3D12_RESIDENCY_PRIORITY prioriry = D3D12_RESIDENCY_PRIORITY_HIGH;
        D3D::Check(g_Device->SetResidencyPriority(1, &heap, &prioriry));
        D3D::Check(g_Device->MakeResident(1, &heap));
    }
}

NAME_METHODS_TEMPLATE(Heap, ID3D12Heap)

Resource* Heap::CreateResource(ResourceFlags flags, ResourceState state, PixelFormat format, u32 width, u32 height, u32 depth, u32 mips, ccstr8 name, i32 nameSize)
{
    u64 size = width * height * RenderTypes::GetPixelSize(format);
    u64 totalSize = 0;

    for (u32 mip = 0; mip < mips; ++mip)
    {
        totalSize += size >> mip;
    }

    totalSize *= depth;

    ED_ASSERT(m_Offset + totalSize < m_Size, "Can not create a resource because exeeded heap capacity.");

    D3D12_RESOURCE_DESC description;
    description.Dimension = depth == 1 ? D3D12_RESOURCE_DIMENSION_TEXTURE2D : D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    description.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    description.Width = width;
    description.Height = height;
    description.DepthOrArraySize = depth;
    description.MipLevels = mips;
    description.Format = DirectX12Types::ConvertPixelFormat(format);
    description.SampleDesc.Count = 1;
    description.SampleDesc.Quality = 0;
    description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    description.Flags = DirectX12Types::ConvertResoruceFlags(flags);

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = DirectX12Types::ConvertPixelFormat(format);
    clearValue.DepthStencil.Depth = 0.0f;
    clearValue.DepthStencil.Stencil = 0;

    uptr resourceHandle;
    D3D::Check(g_Device->CreatePlacedResource(GetHeap(), m_Offset, &description, DirectX12Types::ConvertResourceState(state), flags & RF_AllowDepthStencil ? &clearValue : nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&resourceHandle)));

    Resource* resource = new Resource(flags, state, resourceHandle, format, width, height, depth, mips, name, nameSize);

    if (m_Resident)
    {
        resource->MakeResident();
    }

    m_Resources.Add(resource);

    m_Offset = Align(m_Offset + totalSize, m_Alignment);

    return resource;
}

Resource* Heap::CreateResource(ResourceFlags flags, ResourceState state, PixelFormat format, u32 width, u32 height, ccstr8 name, i32 nameSize)
{
    return CreateResource(flags, state, format, width, height, 1, 1, name, nameSize);
}

Resource* Heap::CreateResource(ResourceFlags flags, ResourceState state, PixelFormat format, u32 width, u32 height, u32 mips, ccstr8 name, i32 nameSize)
{
    return CreateResource(flags, state, format, width, height, 1, mips, name, nameSize);
}

Resource* Heap::CreateResource(ResourceFlags flags, ResourceState state, u32 count, u64 itemSize, ccstr8 name, i32 nameSize)
{
    u32 size = count * itemSize;

    ED_ASSERT(m_Offset + size <= m_Size, "Can not create a resource because exeeded heap capacity.");

    D3D12_RESOURCE_DESC description;
    description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    description.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    description.Width = size;
    description.Height = 1;
    description.DepthOrArraySize = 1;
    description.MipLevels = 1;
    description.Format = DXGI_FORMAT_UNKNOWN;
    description.SampleDesc.Count = 1;
    description.SampleDesc.Quality = 0;
    description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    description.Flags = DirectX12Types::ConvertResoruceFlags(flags);

    uptr resourceHandle;
    D3D::Check(g_Device->CreatePlacedResource(GetHeap(), m_Offset, &description, DirectX12Types::ConvertResourceState(state), nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&resourceHandle)));


    Resource* resource = new Resource(flags, state, resourceHandle, count, itemSize, name, nameSize);
    
    if (m_Resident)
    {
        resource->MakeResident();
    }

    m_Resources.Add(resource);

    m_Offset = Align(m_Offset + size, m_Alignment);

    return resource;
}

void Heap::DeleteResource(Resource* resource)
{
    m_Resources.SwapRemove(resource);
    delete resource;
}

Heap::~Heap()
{
    for (Resource* resource : m_Resources)
    {
        delete resource;
    }
    m_Resources.Clear();

    GetHeap()->Release();
}