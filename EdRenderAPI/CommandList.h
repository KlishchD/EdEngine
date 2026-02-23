#pragma once

class PipelineStateObject;
class DescriptorHeap;
class RootSignature;
class CommandAllocator;

class CommandList {
public:
    CommandList(CommandListType type, ccstr8 name = "DefaultCommandListName");

    void SetDebugName(ccstr8 name);
    void GetDebugName(ccstr8& name);

    void SetDepthBounds(f32 min, f32 max);

    void DrawInstanced(u32 vertexFirst, u32 vertexCount, u32 instancesFirst, u32 instanceCount);
    void Draw(u32 vertexFirst, u32 vertexCount);

    void DrawIndexedInstanced(u32 indexFirst, u32 indexCount, u32 vertexFirst, u32 instancesFirst, u32 instancesCount);
    void DrawIndexed(u32 indexFirst, u32 indexCount, u32 vertexFirst);

    void Dispatch(u32 countX, u32 countY, u32 countZ);

    void CopyBufferRegion(Resource* source, u64 sourceOffset, Resource* destination, u64 destinationOffset, u64 size);
    void CopyTextureRegion(Resource* source, Resource* destination, u32 sourceSubresource, u32 destinationSubresource, glm::u32vec2 sourceOrigin, glm::u32vec2 destinationOrigin, glm::u32vec2 size);
    void CopyTextureRegionFromBuffer(Resource* source, Resource* destination, u64 sourceOffset, u32 width, u32 itemSize, u32 destinationSubresource, glm::u32vec2 destinationOrigin);
    //void CopyTextureRegion(Subresource* source, Subresource* destination, glm::u32vec2 sourceOrigin, glm::u32vec2 destinationOrigin, glm::u32vec2 size);

    void CopyResource(Resource* source, Resource* destination);

    // TODO: CopyTiles

    // TODO: ResolveSubresource for multisampling

    void SetPrimitiveTopology(PrimitiveTopology topology);

    void SetViewports(u32 count, glm::vec4* viewports, f32 minDepth, f32 maxDepth);
    void SetScissors(u32 count, glm::vec4* scissors);
    
    void SetViewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth, f32 maxDepth);
    void SetViewport(glm::vec2 origin, glm::vec2 size, glm::vec2 depthRange);
    void SetScissor(u32 x, u32 y, u32 width, u32 height);
    void SetScissor(glm::u32vec2 origin, glm::u32vec2 size);

    void SetBlendFactor(glm::vec4 factor);

    void SetStencilReference(u8 reference);

    void SetPipelineState(PipelineStateObject* pipeline);

    void ExecuteBundle(CommandList* list);

    void SetDescriptorHeap(DescriptorHeap* heap);
    void SetDescriptorHeap(const TemporaryArray<DescriptorHeap*>& heaps);
    void SetRootSignature(RootSignature* signature, bool comptue);

    void SetComputeRootDescriptorTable(u32 index, u64 origin);
    void SetGraphicsRootDescriptorTable(u32 index, u64 origin);

    void SetComputeRootConstant(u32 index, u32 count, u32 offset, u32* data);
    void SetGraphicsRootConstants(u32 index, u32 count, u32 offset, u32* data);
    
    void SetComputeRootConstant(u32 index, u32 data, u32 offset);
    void SetGraphicsRootConstants(u32 index, u32 data, u32 offset);
    
    void SetGraphicsConstantBufferView(u32 index, Resource* resource);
    void SetGraphicsConstantBufferView(u32 index, Resource* resource, u64 offset);

    void SetGraphicsUnorderedAccessView(u32 index, Resource* resource);
    void SetGraphicsUnorderedAccessView(u32 index, Resource* resource, u64 offset);

    void SetComputeConstantBufferView(u32 index, Resource* resource);
    void SetComputeConstantBufferView(u32 index, Resource* resource, u64 offset);

    void SetComputeUnorderedAccessView(u32 index, Resource* resource);
    void SetComputeUnorderedAccessView(u32 index, Resource* resource, u64 offset);

    /*

        virtual void STDMETHODCALLTYPE SetComputeRootConstantBufferView(
            _In_  UINT RootParameterIndex,
            _In_  D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) = 0;

        virtual void STDMETHODCALLTYPE SetGraphicsRootConstantBufferView(
            _In_  UINT RootParameterIndex,
            _In_  D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) = 0;

        virtual void STDMETHODCALLTYPE SetComputeRootShaderResourceView(
            _In_  UINT RootParameterIndex,
            _In_  D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) = 0;

        virtual void STDMETHODCALLTYPE SetGraphicsRootShaderResourceView(
            _In_  UINT RootParameterIndex,
            _In_  D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) = 0;

        virtual void STDMETHODCALLTYPE SetComputeRootUnorderedAccessView(
            _In_  UINT RootParameterIndex,
            _In_  D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) = 0;

        virtual void STDMETHODCALLTYPE SetGraphicsRootUnorderedAccessView(
            _In_  UINT RootParameterIndex,
            _In_  D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) = 0;
    
    */

    void SetIndexBuffer(Resource* resource);
    void SetVertexBuffer(Resource* resource, u32 slot);

    void SetIndexBuffer(const ResourceView& view);
    void SetVertexBuffer(const ResourceView& view, u32 slot, u32 stride);

    void SetRenderTargets(const ResourceView* target, u32 count, const ResourceView& depth);
    void SetRenderTargets(const ResourceView& target, const ResourceView& depth);

    void ClearDepthTarget(const ResourceView& view, f32 depth);
    void ClearDepthTarget(const ResourceView& view, u8 stencil);
    void ClearDepthTarget(const ResourceView& view, f32 depth, u8 stencil);

    void DiscardResource(const ResourceView& view);
    void ClearRenderTarget(const ResourceView& view, glm::vec4 color);
    
    void ClearUAV(const ResourceView& view, glm::vec4 value);
    void ClearUAV(const ResourceView& view, glm::u32vec4 value);

    void ExecuteIndirect();

    template <u32 count>
    void Transition(Resource* (&resources)[count], ResourceState after)
    {
      Transition(resources, count, after);
    }

    template <u32 count>
    void Transition(ResourceView (&views)[count], ResourceState after)
    {
      Transition(views, count, after);
    }

    void Transition(ResourceView** views, u32 count, ResourceState after);
    void Transition(Resource** resoruces, u32 count, ResourceState after);

    void Transition(ResourceView* views, u32 count, ResourceState after);
    void Transition(Resource* resoruces, u32 count, ResourceState after);

    void Transition(Resource* resource, ResourceState after);
    void Transition(ResourceView& view, ResourceState after);

    void UAVBarrier(Resource* resource);

    void BeginEvent(ccstr8 name);
    void EndEvent();

    CommandListType GetType() const { return m_Type; }

    void Reset();

    void Close();

    template <typename T>
    __forceinline T* GetNativeHandle() const
    {
        return (T*)m_NativeHandle;
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    ~CommandList();
protected:
    uptr m_NativeHandle;

    CommandListType m_Type;

    CommandAllocator* m_Allocator;
};