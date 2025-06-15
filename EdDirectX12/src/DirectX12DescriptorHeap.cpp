#include "EdDirectX12.h"

#define GetHeap() GetNativeHandle<ID3D12DescriptorHeap>()

DescriptorHeap::DescriptorHeap(DescriptorHeapType type, u32 count, bool shaderVisible, ccstr8 name, i32 nameSize) : m_Mask(count), m_Type(type), m_Count(count), m_ShaderVisible(shaderVisible)
{
    ED_ASSERT(type != DescriptorHeapType::Sampler, "Sampler heap is not supported.");
 
    D3D12_DESCRIPTOR_HEAP_DESC description;
    description.Type = DirectX12Types::ConvertDescriptorHeapType(type);
    description.NumDescriptors = count;
    description.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    description.NodeMask = 0;
    D3D::Check(g_Device->CreateDescriptorHeap(&description, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&m_NativeHandle)));

    m_StartLocation = shaderVisible ? GetHeap()->GetGPUDescriptorHandleForHeapStart().ptr : 0;
    m_DescriptorSize = g_Device->GetDescriptorHandleIncrementSize(DirectX12Types::ConvertDescriptorHeapType(m_Type));

    SetDebugName(name, nameSize);
}

NAME_METHODS_TEMPLATE(DescriptorHeap, ID3D12DescriptorHeap)

ResourceView DescriptorHeap::CreateView(DescriptorHeapType type, Resource* resoruce, u32 mip)
{
    ED_ASSERT(resoruce, "Can not create view from nullptr resoruce.");

    i32 slot = -1;

    for (u32 i = 0; i < m_Count; ++i)
    {
        if (!m_Mask.Get(i))
        {
            slot = i;
            break;
        }
    }

    ED_ASSERT(slot >= 0, "Descriptor heap overflow.");

    m_Mask.Set(slot, true);

    u32 descriptorSize = g_Device->GetDescriptorHandleIncrementSize(DirectX12Types::ConvertDescriptorHeapType(type));

    ResourceView view;
    view.Viewed = resoruce;
    view.Type = type;
    view.CPUHandle = GetHeap()->GetCPUDescriptorHandleForHeapStart().ptr + descriptorSize * slot;
    view.GPUHandle = m_ShaderVisible ? GetHeap()->GetGPUDescriptorHandleForHeapStart().ptr + descriptorSize * slot : 0;

    DXGI_FORMAT format = DirectX12Types::ConvertPixelFormat(resoruce->GetPixelFormat());

    switch (type)
    {
    case DescriptorHeapType::CBV:
    {
        D3D12_CONSTANT_BUFFER_VIEW_DESC description;
        description.BufferLocation = resoruce->GetGpuAddress();
        description.SizeInBytes = resoruce->GetSize();
        g_Device->CreateConstantBufferView(&description, { view.CPUHandle });
        break;
    }
    case DescriptorHeapType::SRV:
    {
        if (format == DXGI_FORMAT_D32_FLOAT || format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
        {
            format = DXGI_FORMAT_R32_FLOAT;
        }

        D3D12_SHADER_RESOURCE_VIEW_DESC description;
        description.Format = format;
        description.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        if (resoruce->GetType() == Resource::BufferType)
        {
            description.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            description.Buffer.FirstElement = 0;
            description.Buffer.NumElements = resoruce->GetSize() / resoruce->GetStride();
            description.Buffer.StructureByteStride = resoruce->GetStride();
            description.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        }
        else
        {
            const bool isTexture2D = resoruce->GetDepth() == 1;
            if (isTexture2D)
            {
                description.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                description.Texture2D.MostDetailedMip = 0;
                description.Texture2D.MipLevels = -1;
                description.Texture2D.PlaneSlice = 0;
                description.Texture2D.ResourceMinLODClamp = 0;
            }
            else
            {
                description.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                description.Texture2DArray.MostDetailedMip = 0;
                description.Texture2DArray.MipLevels = -1;
                description.Texture2DArray.FirstArraySlice = 0;
                description.Texture2DArray.ArraySize = resoruce->GetDepth();
                description.Texture2DArray.PlaneSlice = 0;
                description.Texture2DArray.ResourceMinLODClamp = 0;
            }
        }

        g_Device->CreateShaderResourceView(resoruce->GetNativeHandle<ID3D12Resource>(), &description, { view.CPUHandle });

        break;
    }
    case DescriptorHeapType::UAV:
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC description;
        description.Format = format;

        if (resoruce->GetType() == Resource::BufferType)
        {
            description.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            description.Buffer.FirstElement = 0;
            description.Buffer.NumElements = resoruce->GetSize() / resoruce->GetStride();
            description.Buffer.StructureByteStride = resoruce->GetStride();
            description.Buffer.CounterOffsetInBytes = 0;
            description.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
        }
        else
        {
            const bool isTexture2D = resoruce->GetDepth() == 1;
            if (isTexture2D)
            {
                description.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                description.Texture2D.MipSlice = mip;
                description.Texture2D.PlaneSlice = 0;
            }
            else
            {
                description.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                description.Texture2DArray.MipSlice = mip;
                description.Texture2DArray.FirstArraySlice = 0;
                description.Texture2DArray.ArraySize = resoruce->GetDepth();
                description.Texture2DArray.PlaneSlice = 0;
            }
        }

        g_Device->CreateUnorderedAccessView(resoruce->GetNativeHandle<ID3D12Resource>(), nullptr, &description, { view.CPUHandle });
        break;
    }
    case DescriptorHeapType::Sampler:
        ED_ASSERT(0, "Sampler heap is not supported.");
        break;
    case DescriptorHeapType::RTV:
    {
        D3D12_RENDER_TARGET_VIEW_DESC description;
        description.Format = format;

        ED_ASSERT(resoruce->GetType() == Resource::TextureType && resoruce->GetDepth() == 1, "Only textrue 2D RTV's are supported.");

        description.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        description.Texture2D.MipSlice = mip;
        description.Texture2D.PlaneSlice = 0;

        g_Device->CreateRenderTargetView(resoruce->GetNativeHandle<ID3D12Resource>(), &description, { view.CPUHandle });

        break;
    }
    case DescriptorHeapType::DSV:
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC description;
        description.Format = format;

        ED_ASSERT(resoruce->GetType() == Resource::TextureType && resoruce->GetDepth() == 1, "Only textrue 2D DSV's are supported.");

        description.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        description.Flags = D3D12_DSV_FLAG_NONE;
        description.Texture2D.MipSlice = mip;

        g_Device->CreateDepthStencilView(resoruce->GetNativeHandle<ID3D12Resource>(), &description, { view.CPUHandle });
        break;
    }
    default:
        ED_ASSERT(0, "View create for this a descriptor type is not supported {}", (u32)type);
        break;
    }

    return view;
}

ResourceView DescriptorHeap::CreateView(DescriptorHeapType type, Resource* resoruce)
{
    return CreateView(type, resoruce, 0);
}

void DescriptorHeap::FreeView(ResourceView view)
{
    ED_ASSERT(view.Viewed, "Can not free view for nullptr resource.");

    u64 start = GetHeap()->GetCPUDescriptorHandleForHeapStart().ptr;
    u32 descriptorSize = g_Device->GetDescriptorHandleIncrementSize(DirectX12Types::ConvertDescriptorHeapType(m_Type));

    ED_ASSERT(view.CPUHandle >= start && view.CPUHandle <= start + descriptorSize * m_Count, "Can not free view that lies out side of the descriptor heap.");

    u32 slot = (view.CPUHandle - start) / descriptorSize;
    m_Mask.Set(slot, false);
}

u64 DescriptorHeap::GetStartLocation() const
{
    return m_StartLocation;
}

u64 DescriptorHeap::GetDescriptorSize() const
{
    return m_DescriptorSize;
}

u32 DescriptorHeap::GetDescriptorIndex(u64 location) const
{
    return (location - m_StartLocation) / m_DescriptorSize;
}

u32 DescriptorHeap::GetDescriptorIndex(const ResourceView& view) const
{
    return GetDescriptorIndex(view.GPUHandle);
}

void DescriptorHeap::Reserve(u32 slot)
{
    m_Mask.Set(slot, true);
}

u32 DescriptorHeap::ReserveSlot()
{
    u32 slot = m_Count - 1;
    while (slot > 0 && m_Mask.Get(slot))
    {
        --slot;
    }

    ED_ASSERT(!m_Mask.Get(slot), "Exhausted all slots for a descriptor heap.");

    return slot;
}

DescriptorHeap::~DescriptorHeap()
{
    GetHeap()->Release();
}
