#include "EdDirectX12.h"

#define GetList() GetNativeHandle<ID3D12GraphicsCommandList1>()

CommandList::CommandList(CommandListType type, ccstr8 name) : m_Type(type)
{
    m_Allocator = new CommandAllocator(type);

    D3D::Check(g_Device->CreateCommandList(0, 
        DirectX12Types::ConvertCommandListType(type), 
        m_Allocator->GetNativeHandle<ID3D12CommandAllocator>(), 
        nullptr, 
        __uuidof(ID3D12GraphicsCommandList1), 
        reinterpret_cast<void**>(&m_NativeHandle)));

    SetDebugName(name);

    GetList()->Close();
}

NAME_METHODS_TEMPLATE(CommandList, ID3D12CommandList)

void CommandList::SetDepthBounds(f32 min, f32 max)
{
    GetList()->OMSetDepthBounds(min, max);
}

void CommandList::DrawInstanced(u32 vertexFirst, u32 vertexCount, u32 instancesFirst, u32 instanceCount)
{
    GetList()->DrawInstanced(vertexCount, instanceCount, vertexFirst, instancesFirst);
}

void CommandList::Draw(u32 vertexFirst, u32 vertexCount)
{
    GetList()->DrawInstanced(vertexCount, 1, vertexFirst, 0);
}

void CommandList::DrawIndexedInstanced(u32 indexFirst, u32 indexCount, u32 vertexFirst, u32 instancesFirst, u32 instancesCount)
{
    GetList()->DrawIndexedInstanced(indexCount, instancesCount, indexFirst, vertexFirst, instancesFirst);
}

void CommandList::DrawIndexed(u32 indexFirst, u32 indexCount, u32 vertexFirst)
{
    GetList()->DrawIndexedInstanced(indexCount, 1, indexFirst, vertexFirst, 0);
}

void CommandList::Dispatch(u32 countX, u32 countY, u32 countZ)
{
    GetList()->Dispatch(countX, countY, countZ);
}

void CommandList::CopyBufferRegion(Resource* source, u64 sourceOffset, Resource* destination, u64 destinationOffset, u64 size)
{
    ED_ASSERT(source, "Can not copy resoruce from nullptr.");
    ED_ASSERT(destination, "Can not copy to nullptr resource.");
    ED_ASSERT(source->GetType() == Resource::BufferType && destination->GetType() == Resource::BufferType, "Copy source and distination must be both buffers.");

    ID3D12Resource* destiantionRaw = destination->GetNativeHandle<ID3D12Resource>();
    ID3D12Resource* sourceRaw = source->GetNativeHandle<ID3D12Resource>();

    GetList()->CopyBufferRegion(destiantionRaw, destinationOffset, sourceRaw, sourceOffset, size);
}

void CommandList::CopyTextureRegion(Resource* source, Resource* destination, u32 sourceSubresource, u32 destinationSubresource, glm::u32vec2 sourceOrigin, glm::u32vec2 destinationOrigin, glm::u32vec2 size)
{
    ED_ASSERT(source, "Can not copy resoruce from nullptr.");
    ED_ASSERT(destination, "Can not copy to nullptr resource.");
    ED_ASSERT(source->GetType() == Resource::TextureType && destination->GetType() == Resource::TextureType, "Copy source and distination must be both textures.");
    ED_ASSERT(sourceSubresource < source->GetSubresourcesCount(), "Can not copy from textrue subresource that doesn't exist, tried {} but present {}", sourceSubresource, source->GetSubresourcesCount());
    ED_ASSERT(destinationSubresource < destination->GetSubresourcesCount(), "Can not copy to textrue subresource that doesn't exist, tried {} but present {}", destinationSubresource, destination->GetSubresourcesCount());

    D3D12_TEXTURE_COPY_LOCATION sourceLocation;
    sourceLocation.pResource = source->GetNativeHandle<ID3D12Resource>();
    sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    sourceLocation.SubresourceIndex = sourceSubresource;

    D3D12_TEXTURE_COPY_LOCATION destinationLocation;
    destinationLocation.pResource = destination->GetNativeHandle<ID3D12Resource>();
    destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    destinationLocation.SubresourceIndex = destinationSubresource;

    D3D12_BOX box;
    box.left = sourceOrigin.x;
    box.right = sourceOrigin.x + size.x;
    box.top = sourceOrigin.y;
    box.bottom = sourceOrigin.y + size.y;
    box.front = 0;
    box.back = 1;

    GetList()->CopyTextureRegion(&destinationLocation, destinationOrigin.x, destinationOrigin.y, 0, &sourceLocation, &box);
}

void CommandList::CopyTextureRegionFromBuffer(Resource* source, Resource* destination, u64 sourceOffset, u32 width, u32 itemSize, u32 destinationSubresource, glm::u32vec2 destinationOrigin)
{
    ED_ASSERT(source, "Can not copy resoruce from nullptr.");
    ED_ASSERT(destination, "Can not copy to nullptr resource.");
    ED_ASSERT(source->GetType() == Resource::BufferType, "Copy source must be buffer.");
    ED_ASSERT(destination->GetType() == destination->GetType() == Resource::TextureType, "Copy destination must be texture.");
    ED_ASSERT(destinationSubresource < destination->GetSubresourcesCount(), "Can not copy to textrue subresource that doesn't exist, tried {} but present {}", destinationSubresource, destination->GetSubresourcesCount());

    D3D12_TEXTURE_COPY_LOCATION sourceLocation;
    sourceLocation.pResource = source->GetNativeHandle<ID3D12Resource>();
    sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    sourceLocation.PlacedFootprint.Offset = sourceOffset;
    sourceLocation.PlacedFootprint.Footprint.Format = DirectX12Types::ConvertPixelFormat(destination->GetPixelFormat());
    sourceLocation.PlacedFootprint.Footprint.Width = destination->GetWidth();
    sourceLocation.PlacedFootprint.Footprint.Height = destination->GetHeight();
    sourceLocation.PlacedFootprint.Footprint.Depth = destination->GetDepth();
    sourceLocation.PlacedFootprint.Footprint.RowPitch = destination->GetWidth() * RenderTypes::GetPixelSize(destination->GetPixelFormat());

    D3D12_TEXTURE_COPY_LOCATION destinationLocation;
    destinationLocation.pResource = destination->GetNativeHandle<ID3D12Resource>();
    destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    destinationLocation.SubresourceIndex = destinationSubresource;

    GetList()->CopyTextureRegion(&destinationLocation, destinationOrigin.x, destinationOrigin.y, 0, &sourceLocation, nullptr);
}

// 
// void CommandList::CopyTextureRegion(Subresource* source, Subresource* destination, glm::u32vec2 sourceOrigin, glm::u32vec2 destinationOrigin, glm::u32vec2 size)
// {
//     ED_ASSERT(source, "Can not copy resoruce from nullptr.");
//     ED_ASSERT(destination, "Can not copy to nullptr resource.");
//     ED_ASSERT(source->GetType() == Resource::TextureType && destination->GetType() == Resource::TextureType, "Copy source and distination must be both textures.");
//     ED_ASSERT(size.x < source->GetWidth() && size.x < destination->GetWidth(), "Can not copy from/to more than the texture can hold.");
//     ED_ASSERT(size.y < source->GetHeight() && size.y < destination->GetHeight(), "Can not copy from/to more than the texture can hold.");
// 
//     u32 sourceFormatSize = RenderTypes::GetPixelSize(source->GetPixelFormat());
//     u32 destinationFormatSize = RenderTypes::GetPixelSize(destination->GetPixelFormat());
// 
//     D3D12_TEXTURE_COPY_LOCATION sourceLocation;
//     sourceLocation.pResource = destination->GetNativeHandle<ID3D12Resource>();
//     sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
//     sourceLocation.PlacedFootprint.Offset = source->GetOffset();
//     sourceLocation.PlacedFootprint.Footprint.Format = DirectX12Types::ConvertPixelFormat(source->GetPixelFormat());
//     sourceLocation.PlacedFootprint.Footprint.Width = source->GetWidth();
//     sourceLocation.PlacedFootprint.Footprint.Height = source->GetHeight();
//     sourceLocation.PlacedFootprint.Footprint.Depth = source->GetDepth();
//     sourceLocation.PlacedFootprint.Footprint.RowPitch = source->GetWidth() * sourceFormatSize;
// 
//     D3D12_TEXTURE_COPY_LOCATION destinationLocation;
//     destinationLocation.pResource = destination->GetNativeHandle<ID3D12Resource>();
//     destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
//     destinationLocation.PlacedFootprint.Offset = destination->GetOffset();
//     destinationLocation.PlacedFootprint.Footprint.Format = DirectX12Types::ConvertPixelFormat(destination->GetPixelFormat());
//     destinationLocation.PlacedFootprint.Footprint.Width = destination->GetWidth();
//     destinationLocation.PlacedFootprint.Footprint.Height = destination->GetHeight();
//     destinationLocation.PlacedFootprint.Footprint.Depth = destination->GetDepth();
//     destinationLocation.PlacedFootprint.Footprint.RowPitch = destination->GetWidth() * destinationFormatSize;
// 
//     D3D12_BOX box;
//     box.left = sourceOrigin.x;
//     box.right = sourceOrigin.x + size.x;
//     box.top = sourceOrigin.y;
//     box.bottom = sourceOrigin.y + size.y;
//     box.front = 0;
//     box.back = 1;
// 
//     GetList()->CopyTextureRegion(&destinationLocation, destinationOrigin.x, destinationOrigin.y, 0, &sourceLocation, &box);
// }

void CommandList::CopyResource(Resource* source, Resource* destination)
{
    ED_ASSERT(source, "Can not copy resoruce from nullptr.");
    ED_ASSERT(destination, "Can not copy to nullptr resource.");

    GetList()->CopyResource(destination->GetNativeHandle<ID3D12Resource>(), source->GetNativeHandle<ID3D12Resource>());
}

void CommandList::SetPrimitiveTopology(PrimitiveTopology topology)
{
    GetList()->IASetPrimitiveTopology(DirectX12Types::ConverPrimitiveTopology(topology));
}

void CommandList::SetViewports(u32 count, glm::vec4* viewports, f32 minDepth, f32 maxDepth)
{
    ED_ASSERT(count <= 16, "Can not set more than 16 viewports.");

    D3D12_VIEWPORT converted[16];
    for (u32 i = 0; i < count; ++i)
    {
        converted[i].TopLeftX = viewports[i].x;
        converted[i].TopLeftY = viewports[i].y;
        converted[i].Width = viewports[i].z;
        converted[i].Height = viewports[i].w;
        converted[i].MinDepth = minDepth;
        converted[i].MaxDepth = maxDepth;
    }

    GetList()->RSSetViewports(count, converted);
}

void CommandList::SetScissors(u32 count, glm::vec4* scissors)
{
    ED_ASSERT(count <= 16, "Can not set more than 16 scissors.");

    D3D12_RECT converted[16];
    for (u32 i = 0; i < count; ++i)
    {
        converted[i].left = static_cast<u32>(scissors[i].x);
        converted[i].top = static_cast<u32>(scissors[i].y);
        converted[i].right = static_cast<u32>(scissors[i].x + scissors[i].z);
        converted[i].bottom = static_cast<u32>(scissors[i].y + scissors[i].w);
    }

    GetList()->RSSetScissorRects(count, converted);
}

void CommandList::SetViewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth, f32 maxDepth)
{
    D3D12_VIEWPORT viewport;
    viewport.TopLeftX = x;
    viewport.TopLeftY = y;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = minDepth;
    viewport.MaxDepth = maxDepth;
    GetList()->RSSetViewports(1, &viewport);
}

void CommandList::SetViewport(glm::vec2 origin, glm::vec2 size, glm::vec2 depthRange)
{
    D3D12_VIEWPORT viewport;
    viewport.TopLeftX = origin.x;
    viewport.TopLeftY = origin.y;
    viewport.Width = size.x;
    viewport.Height = size.y;
    viewport.MinDepth = depthRange.x;
    viewport.MaxDepth = depthRange.y;
    GetList()->RSSetViewports(1, &viewport);
}

void CommandList::SetScissor(u32 x, u32 y, u32 width, u32 height)
{
    D3D12_RECT rect;
    rect.left = x;
    rect.right = x + width;
    rect.top = y;
    rect.bottom = y + height;
    GetList()->RSSetScissorRects(1, &rect);
}

void CommandList::SetScissor(glm::u32vec2 origin, glm::u32vec2 size)
{
    D3D12_RECT rect;
    rect.left = origin.x;
    rect.right = origin.x + size.x;
    rect.top = origin.y;
    rect.bottom = origin.y + size.y;
    GetList()->RSSetScissorRects(1, &rect);
}

void CommandList::SetBlendFactor(glm::vec4 factor)
{
    GetList()->OMSetBlendFactor(glm::value_ptr(factor));
}

void CommandList::SetStencilReference(u8 reference)
{
    GetList()->OMSetStencilRef(reference);
}

void CommandList::SetPipelineState(PipelineStateObject* pipeline)
{
    ED_ASSERT(pipeline, "Can not set nullptr pipeline state object.");

    GetList()->SetPipelineState(pipeline->GetNativeHandle<ID3D12PipelineState>());
}

void CommandList::ExecuteBundle(CommandList* list)
{
    ED_ASSERT(list, "Can not execure nullptr bundle command list.");

    GetList()->ExecuteBundle(list->GetNativeHandle<ID3D12GraphicsCommandList>());
}

void CommandList::SetDescriptorHeap(DescriptorHeap* heap)
{
    ED_ASSERT(heap, "Can not set nullptr descriptor heap.");

    ID3D12DescriptorHeap* native = heap->GetNativeHandle<ID3D12DescriptorHeap>();
    GetList()->SetDescriptorHeaps(1, &native);
}

void CommandList::SetDescriptorHeap(const TemporaryArray<DescriptorHeap*>& heaps)
{
    ID3D12DescriptorHeap** native = Memory::Get().RequestDynamicMemory<ID3D12DescriptorHeap*>(heaps.GetSize(), 1, "D3D12Heaps");

    for (u32 i = 0; i < heaps.GetSize(); ++i)
    {
        ED_ASSERT(heaps[i], "Can not set nullptr descriptor heap.");
        native[i] = heaps[i]->GetNativeHandle<ID3D12DescriptorHeap>();
    }

    GetList()->SetDescriptorHeaps(heaps.GetSize(), native);
}

void CommandList::SetRootSignature(RootSignature* signature, bool comptue)
{
    ED_ASSERT(signature, "Can not set nullptr root signature.");

    if (comptue)
    {
        GetList()->SetComputeRootSignature(signature->GetNativeHandle<ID3D12RootSignature>());
    }
    else
    {
        GetList()->SetGraphicsRootSignature(signature->GetNativeHandle<ID3D12RootSignature>());
    }
}

void CommandList::SetComputeRootDescriptorTable(u32 index, u64 origin)
{
    GetList()->SetComputeRootDescriptorTable(index, { origin });
}

void CommandList::SetGraphicsRootDescriptorTable(u32 index, u64 origin)
{
    GetList()->SetGraphicsRootDescriptorTable(index, { origin });
}

void CommandList::SetComputeRootConstant(u32 index, u32 count, u32 offset, u32* data)
{
    GetList()->SetComputeRoot32BitConstants(index, count, data, offset);
}

void CommandList::SetGraphicsRootConstants(u32 index, u32 count, u32 offset, u32* data)
{
    GetList()->SetGraphicsRoot32BitConstants(index, count, data, offset);
}

void CommandList::SetComputeRootConstant(u32 index, u32 data, u32 offset)
{
    GetList()->SetComputeRoot32BitConstant(index, data, offset);
}

void CommandList::SetGraphicsRootConstants(u32 index, u32 data, u32 offset)
{
    GetList()->SetGraphicsRoot32BitConstant(index, data, offset);
}

void CommandList::SetGraphicsConstantBufferView(u32 index, Resource* resource)
{
    GetList()->SetGraphicsRootConstantBufferView(index, resource->GetGpuAddress());
}

void CommandList::SetGraphicsConstantBufferView(u32 index, Resource* resource, u64 offset)
{
    GetList()->SetGraphicsRootConstantBufferView(index, resource->GetGpuAddress() + offset);
}

void CommandList::SetGraphicsUnorderedAccessView(u32 index, Resource* resource)
{
    GetList()->SetGraphicsRootUnorderedAccessView(index, resource->GetGpuAddress());
}

void CommandList::SetGraphicsUnorderedAccessView(u32 index, Resource* resource, u64 offset)
{
    GetList()->SetGraphicsRootUnorderedAccessView(index, resource->GetGpuAddress() + offset);
}

void CommandList::SetComputeUnorderedAccessView(u32 index, Resource* resource)
{
    GetList()->SetComputeRootUnorderedAccessView(index, resource->GetGpuAddress());
}

void CommandList::SetComputeUnorderedAccessView(u32 index, Resource* resource, u64 offset)
{
    GetList()->SetComputeRootUnorderedAccessView(index, resource->GetGpuAddress() + offset);
}

void CommandList::SetComputeConstantBufferView(u32 index, Resource* resource)
{
    GetList()->SetComputeRootConstantBufferView(index, resource->GetGpuAddress());
}

void CommandList::SetComputeConstantBufferView(u32 index, Resource* resource, u64 offset)
{
    GetList()->SetComputeRootConstantBufferView(index, resource->GetGpuAddress() + offset);
}

void CommandList::SetIndexBuffer(Resource* resource)
{
    ED_ASSERT(resource, "Can not set nullptr index buffer.");
    ED_ASSERT(resource->GetType() == Resource::BufferType, "Can only set buffer as index buffer.");

    D3D12_INDEX_BUFFER_VIEW view;
    view.BufferLocation = resource->GetGpuAddress();
    view.SizeInBytes = resource->GetSize();
    view.Format = DXGI_FORMAT_R32_UINT;
    GetList()->IASetIndexBuffer(&view);
}

void CommandList::SetVertexBuffer(Resource* resource, u32 slot)
{
    ED_ASSERT(resource, "Can not set nullptr vertex buffer.");
    ED_ASSERT(resource->GetType() == Resource::BufferType, "Can only set buffer as vertex buffer.");

    D3D12_VERTEX_BUFFER_VIEW view;
    view.BufferLocation = resource->GetGpuAddress();
    view.SizeInBytes = resource->GetSize();
    view.StrideInBytes = resource->GetStride();
    GetList()->IASetVertexBuffers(slot, 1, &view);
}

void CommandList::SetIndexBuffer(const ResourceView& view)
{
    ED_ASSERT(view.Viewed, "Can not set nullptr index buffer.");
    ED_ASSERT(view.Viewed->GetType() == Resource::BufferType, "Can only set buffer as index buffer.");
    ED_ASSERT(view.Size > 0, "Index buffer view size must be more than 0.");

    D3D12_INDEX_BUFFER_VIEW viewDescription;
    viewDescription.BufferLocation = view.GPUHandle;
    viewDescription.SizeInBytes = view.Size;
    viewDescription.Format = DXGI_FORMAT_R32_UINT;
    GetList()->IASetIndexBuffer(&viewDescription);
}

void CommandList::SetVertexBuffer(const ResourceView& view, u32 slot, u32 stride)
{
    ED_ASSERT(view.Viewed, "Can not set nullptr vertex buffer.");
    ED_ASSERT(view.Viewed->GetType() == Resource::BufferType, "Can only set buffer as vertex buffer.");
    ED_ASSERT(view.Size > 0, "Vertex buffer view size must be more than 0.");

    D3D12_VERTEX_BUFFER_VIEW viewDescription;
    viewDescription.BufferLocation = view.GPUHandle;
    viewDescription.SizeInBytes = view.Size;
    viewDescription.StrideInBytes = stride;
    GetList()->IASetVertexBuffers(slot, 1, &viewDescription);
}

void CommandList::SetRenderTargets(const TemporaryArray<ResourceView>& targets, const ResourceView& depth)
{
    D3D12_CPU_DESCRIPTOR_HANDLE* nativeTargets = Memory::Get().RequestDynamicMemory<D3D12_CPU_DESCRIPTOR_HANDLE>(targets.GetSize(), 1, "RenderTargetsHandles");

    for (u32 i = 0; i < targets.GetSize(); ++i)
    {
        ED_ASSERT(targets[i].Viewed->GetState(m_Type, 0) == ResourceState::RenderTarget, "Expected target to be in render target state.");
        nativeTargets[i] = { targets[i].CPUHandle };
    }

    if (depth.Viewed)
    {
        ED_ASSERT(depth.Viewed->GetState(m_Type, 0) == ResourceState::DepthWrite, "Expected depth target to be in depth write state.");

        D3D12_CPU_DESCRIPTOR_HANDLE depthHandle = { depth.CPUHandle };
        GetList()->OMSetRenderTargets(targets.GetSize(), nativeTargets, false, &depthHandle);
    }
    else
    {
        GetList()->OMSetRenderTargets(targets.GetSize(), nativeTargets, false, nullptr);
    }
}

void CommandList::SetRenderTargets(const ResourceView& target, const ResourceView& depth)
{
    D3D12_CPU_DESCRIPTOR_HANDLE nativeTarget = { target.CPUHandle };
    D3D12_CPU_DESCRIPTOR_HANDLE depthHandle = { depth.CPUHandle };

    GetList()->OMSetRenderTargets(target.Viewed ? 1 : 0, target.Viewed ? &nativeTarget : nullptr, false, depth.Viewed ? &depthHandle : nullptr);
}

void CommandList::DiscardResource(const ResourceView& view)
{
  GetList()->DiscardResource(view.Viewed->GetNativeHandle<ID3D12Resource>(), nullptr);
}

void CommandList::ClearDepthTarget(const ResourceView& view, f32 depth)
{
    GetList()->ClearDepthStencilView({ view.CPUHandle }, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void CommandList::ClearDepthTarget(const ResourceView& view, u8 stencil)
{
    GetList()->ClearDepthStencilView({ view.CPUHandle }, D3D12_CLEAR_FLAG_STENCIL, 0, stencil, 0, nullptr);
}

void CommandList::ClearDepthTarget(const ResourceView& view, f32 depth, u8 stencil)
{
    GetList()->ClearDepthStencilView({ view.CPUHandle }, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
}

void CommandList::ClearRenderTarget(const ResourceView& view, glm::vec4 color)
{
    GetList()->ClearRenderTargetView({ view.CPUHandle }, glm::value_ptr(color), 0, nullptr);
}

void CommandList::ClearUAV(const ResourceView& view, glm::vec4 value)
{
    GetList()->ClearUnorderedAccessViewFloat({ view.GPUHandle }, { view.CPUHandle }, view.Viewed->GetNativeHandle<ID3D12Resource>(), glm::value_ptr(value), 0, nullptr);
}

void CommandList::ClearUAV(const ResourceView& view, glm::u32vec4 value)
{
    GetList()->ClearUnorderedAccessViewUint({ view.GPUHandle }, { view.CPUHandle }, view.Viewed->GetNativeHandle<ID3D12Resource>(), glm::value_ptr(value), 0, nullptr);
}

void CommandList::ExecuteIndirect()
{
    ED_ASSERT(0, "Not yet implemented !!!");
}

void CommandList::Transition(const TemporaryArray<Resource*>& resources, ResourceState after)
{
    ED_ASSERT(resources.GetSize(), "Must transition at least one target.");

    u32 count = 0;

    for (u32 i = 0; i < resources.GetSize(); ++i)
    {
        ED_ASSERT(resources[i], "Can not transition nullptr resoruce.");
        count += resources[i]->GetSubresourcesCount();
    }

    D3D12_RESOURCE_BARRIER* barriers = Memory::Get().RequestDynamicMemory<D3D12_RESOURCE_BARRIER>(count, 1, "D3D12Barriers");

    u32 offset = 0;
    for (u32 i = 0; i < resources.GetSize(); ++i)
    {
        for (u32 j = 0; j < resources[i]->GetSubresourcesCount(); ++j)
        {
            if (resources[i]->GetState(m_Type, j) == after)
            {
                continue;
            }

            barriers[offset].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barriers[offset].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[offset].Transition.pResource = resources[i]->GetNativeHandle<ID3D12Resource>();
            barriers[offset].Transition.Subresource = j;
            barriers[offset].Transition.StateBefore = DirectX12Types::ConvertResourceState(resources[i]->GetState(m_Type, j));
            barriers[offset].Transition.StateAfter = DirectX12Types::ConvertResourceState(after);
            resources[i]->SetState(m_Type, j, after);

            ++offset;
        }
    }

    if (offset > 0)
    {
        GetList()->ResourceBarrier(offset, barriers);
    }
}

void CommandList::Transition(const TemporaryArray<ResourceView>& views, ResourceState after)
{
    TemporaryArray<Resource*> resources(views.GetSize());

    for (u32 i = 0; i < views.GetSize(); ++i)
    {
        resources.Add(views[i].Viewed);
    }

    Transition(resources, after);
}

void CommandList::Transition(Resource* resource, ResourceState after)
{
    ED_ASSERT(resource, "Can not transition nullptr resoruce.");

    u32 subresourcesCount = resource->GetSubresourcesCount();
    D3D12_RESOURCE_BARRIER* barriers = Memory::Get().RequestDynamicMemory<D3D12_RESOURCE_BARRIER>(subresourcesCount, 1, "D3D12Barriers");

    u32 count = 0;
    for (u32 i = 0; i < resource->GetSubresourcesCount(); ++i)
    {
        if (resource->GetState(m_Type, i) == after)
        {
            continue;
        }

        barriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barriers[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barriers[i].Transition.pResource = resource->GetNativeHandle<ID3D12Resource>();
        barriers[i].Transition.Subresource = i;
        barriers[i].Transition.StateBefore = DirectX12Types::ConvertResourceState(resource->GetState(m_Type, i));
        barriers[i].Transition.StateAfter = DirectX12Types::ConvertResourceState(after);
        resource->SetState(m_Type, i, after);

        ++count;
    }

    if (count > 0)
    {
        GetList()->ResourceBarrier(count, barriers);
    }
}

void CommandList::Transition(const ResourceView& view, ResourceState after)
{
    Transition(view.Viewed, after);
}

void CommandList::UAVBarrier(Resource* resource)
{
    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.UAV.pResource = resource->GetNativeHandle<ID3D12Resource>();
    GetList()->ResourceBarrier(1, &barrier);
}

void CommandList::BeginEvent(ccstr8 name)
{
#if D3D_DEBUG == 0 && D3D_FORCE_EVENTS_OFF == 0
    GetList()->BeginEvent(1, name, strnlen(name, 1024));
#endif
}

void CommandList::EndEvent()
{
#if D3D_DEBUG == 0 && D3D_FORCE_EVENTS_OFF == 0
    GetList()->EndEvent();
#endif
}

void CommandList::Reset()
{
    m_Allocator->Reset();
    D3D::Check(GetList()->Reset(m_Allocator->GetNativeHandle<ID3D12CommandAllocator>(), nullptr));
}

void CommandList::Close()
{
    D3D::Check(GetList()->Close());
}

CommandList::~CommandList()
{
    delete m_Allocator;
    GetList()->Release();
}
