#include "EdRenderApiPrivate.h"
#include "RenderingContext.h"

void CustomResourceAllocator::Initialize(Resource* resource)
{
    m_Resource = resource;
    m_Offset = 0;
}

u64 CustomResourceAllocator::Allocate(u64 size)
{
    ED_ASSERT(m_Offset + size < m_Resource->GetSize(), "Can not allocate more space than resource supports");

    u64 offset = m_Offset;
    m_Offset += size;
    return offset;
}

void CustomResourceAllocator::Deallocate(u64 offset)
{
    ED_LOG(CustomResourceAllocator, warn, "CustomResourceAllocator::Deallocate is not implemented.");
}

void RenderingContext::UploadModelDeferred(void* vertices, u32 vertexSize, u32 vertexCount, ResourceView& vertexBufferView, u32* indices, u32 indexCount, ResourceView& indexBufferView)
{
    UploadRequest vertexRequest;
    vertexRequest.Destiantion = &vertexBufferView;
    vertexRequest.Data = vertices;
    vertexRequest.Width = vertexCount;
    vertexRequest.Height = 1;
    vertexRequest.Mips = 1;
    vertexRequest.ItemSize = vertexSize;
    vertexRequest.Offset = m_GeometryVertexPool.Allocate(vertexRequest.GetSize());
    vertexRequest.State = ResourceState::Common;
    m_UploadRequests[UD_VertexPool].Add(vertexRequest);

    vertexBufferView.Viewed = m_GeometryVertexPool.GetResource();
    vertexBufferView.GPUHandle = m_GeometryVertexPool.GetResource()->GetGpuAddress() + vertexRequest.Offset;
    vertexBufferView.Size = vertexSize * vertexCount;

    UploadRequest indexRequset;
    indexRequset.Destiantion = &indexBufferView;
    indexRequset.Data = indices;
    indexRequset.Width = indexCount;
    indexRequset.Height = 1;
    indexRequset.Mips = 1;
    indexRequset.ItemSize = sizeof(u32);
    indexRequset.Offset = m_GeometryIndexPool.Allocate(indexRequset.GetSize());
    indexRequset.State = ResourceState::Common;
    m_UploadRequests[UD_IndexPool].Add(indexRequset);

    indexBufferView.Viewed = m_GeometryIndexPool.GetResource();
    indexBufferView.GPUHandle = m_GeometryIndexPool.GetResource()->GetGpuAddress() + indexRequset.Offset;
    indexBufferView.Size = indexCount * sizeof(u32);
}

void RenderingContext::UploadModelImediate(void* vertices, u32 vertexSize, u32 vertexCount, ResourceView& vertexBufferView, u32* indices, u32 indexCount, ResourceView& indexBufferView)
{
    ED_ASSERT(0, "RenderingContext::UploadModelImediate is not implemented.");
}

void RenderingContext::FreeModel(ResourceView& vertexBufferView, ResourceView& indexBufferView)
{
    ED_LOG(CustomResourceAllocator, warn, "Implement this method with move to allocators.");
    vertexBufferView = ResourceView();
    indexBufferView = ResourceView();
}

void RenderingContext::UploadTextureDeferred(u8* data, u32 width, u32 height, u32 mips, PixelFormat format, ResourceView& view, ResourceState state, ccstr8 name, i32 nameSize)
{
    UploadRequest request;
    request.Destiantion = &view;
    request.Data = data;
    request.Width = width;
    request.Height = height;
    request.Mips = mips;
    request.ItemSize = RenderTypes::GetPixelSize(format);
    request.Offset = 0;
    request.State = state;
    m_UploadRequests[UD_TexturePool].Add(request);

    Resource* resource = m_TexturesHeap->CreateResource(RF_None, ResourceState::ShaderRead, format, width, height, mips, name, nameSize);
 
    view = m_SRVHeap->CreateView(DescriptorHeapType::SRV, resource);
}

void RenderingContext::UploadTextureImediate(u8* Data, u32 width, u32 height, u32 mips, PixelFormat format, ResourceView& view, ResourceState state, ccstr8 name, i32 nameSize)
{
    ED_ASSERT(0, "RenderingContext::UploadTextureImediate is not implemented.");
}

void RenderingContext::FreeTexture(ResourceView& view)
{
    ED_ASSERT(view.Viewed, "Can not free nullptr resource.");
    ED_ASSERT(view.Type == DescriptorHeapType::SRV, "FreeTexture expects SRV to free.");
    m_TexturesHeap->DeleteResource(view.Viewed);
    view = ResourceView();
}

ResourceView RenderingContext::CreateRenderTarget(RenderTargetSizePolicy policy, PixelFormat format, ccstr8 name, i32 nameSize)
{
    f32 scaler = RenderTypes::ConvertRenderTargetSizePolicy(policy);
    u32 width = static_cast<u32>(m_Window->GetWidth() * scaler);
    u32 height = static_cast<u32>(m_Window->GetHeight() * scaler);

    return CreateRenderTarget(width, height, format, name, nameSize);
}

ResourceView RenderingContext::CreateRenderTarget(u32 width, u32 height, PixelFormat format, ccstr8 name, i32 nameSize)
{
    ResourceView view;

    if (format == PixelFormat::Depth || format == PixelFormat::DepthStencil)
    {
        Resource* target = m_RenderTargetsHeap->CreateResource(RF_AllowDepthStencil, ResourceState::DepthWrite, format, width, height, name, nameSize);
        view = m_DSVHeap->CreateView(DescriptorHeapType::DSV, target);
    }
    else
    {
        Resource* target = m_RenderTargetsHeap->CreateResource(RF_AllowRenderTarget, ResourceState::RenderTarget, format, width, height, name, nameSize);
        view = m_RTVHeap->CreateView(DescriptorHeapType::RTV, target);
    }
    
    return view;
}

void RenderingContext::FreeRenderTarget(ResourceView& view)
{
    ED_ASSERT(view.Viewed, "Can not free nullptr resource.");
    ED_ASSERT(view.Type == DescriptorHeapType::RTV, "FreeRenderTarget expects RTV to free.");
    m_RenderTargetsHeap->DeleteResource(view.Viewed);
    view = ResourceView();
}

Array<ResourceView> RenderingContext::CreateUAVTarget(RenderTargetSizePolicy policy, u32 mips, PixelFormat format, ccstr8 name, i32 nameSize)
{
    f32 scaler = RenderTypes::ConvertRenderTargetSizePolicy(policy);
    u32 width = static_cast<u32>(m_Window->GetWidth() * scaler);
    u32 height = static_cast<u32>(m_Window->GetHeight() * scaler);

    return CreateUAVTarget(width, height, mips, format, name, nameSize);
}

Array<ResourceView> RenderingContext::CreateUAVTarget(u32 width, u32 height, u32 mips, PixelFormat format, ccstr8 name, i32 nameSize)
{
    Resource* target = m_UAVTargetsHeap->CreateResource(RF_AllowUnorderedAccess, ResourceState::UnorderedAccess, format, width, height, mips, name, nameSize);
    
    Array<ResourceView> views;
    for (u32 i = 0; i < mips; ++i)
    {
        ResourceView& view = views.Add();
        view = m_UAVHeap->CreateView(DescriptorHeapType::UAV, target, i);
    }

    return views;
}

void RenderingContext::FreeUAVTarget(ResourceView& view)
{
    ED_ASSERT(view.Viewed, "Can not free nullptr resource.");
    ED_ASSERT(view.Type == DescriptorHeapType::UAV, "FreeUAVTarget expects UAV to free.");
    m_UAVTargetsHeap->DeleteResource(view.Viewed);
    view = ResourceView();
}

void RenderingContext::FreeUAVTargets(Array<ResourceView>& views)
{
    for (ResourceView& view : views)
    {
        FreeUAVTarget(view);
    }
}

Shader* RenderingContext::CreateShader(const ContentPath& path, ShaderType type)
{
    Shader* shader = new Shader(type, path);
    m_Shaders.Add(shader);
    return shader;
}

ShaderProgram* RenderingContext::CreateShaderProgram(const ContentPath& vertex, const ContentPath& pixel)
{
    ShaderProgram* program = CreateShaderProgram();
    program->AttachShader(vertex, ST_Vertex);
    program->AttachShader(pixel, ST_Pixel);
    return program;
}

ShaderProgram* RenderingContext::CreateShaderProgram()
{
    ShaderProgram* program = new ShaderProgram();
    m_ShaderPrograms.Add(program);
    return program;
}

ShaderProgram* RenderingContext::CreateShaderProgram(const ContentPath& path, ShaderType types)
{
    ShaderProgram* program = CreateShaderProgram();

    for (u32 i = 0; i < ST_Count; ++i)
    {
        ShaderType type = static_cast<ShaderType>(1 << i);
        if (types & type)
        {
            program->AttachShader(CreateShader(path, type));
        }
    }

    return program;
}

RootSignature* RenderingContext::CreateRootSignature(const RootSignatureBuilder& builder)
{
    RootSignature* signature = new RootSignature(builder);
    m_RootSignatures.Add(signature);
    return signature;
}

PipelineStateObject* RenderingContext::CreatePipelineStateObject(ccstr8 name, const GraphicsPipelineStateObjectBuilder& builder)
{
    PipelineStateObject* pso = new PipelineStateObject(name, builder);
    m_PSOs.Add(pso);
    return pso;
}

PipelineStateObject* RenderingContext::CreatePipelineStateObject(ccstr8 name, RootSignature* signature, ShaderProgram* program)
{
    PipelineStateObject* pso = new PipelineStateObject(name, signature, program);
    m_PSOs.Add(pso);
    return pso;
}

PipelineStateObject* RenderingContext::CreatePipelineStateObject(ccstr8 name, RootSignature* signature, const ContentPath& path)
{
    ShaderProgram* program = CreateShaderProgram();
    program->AttachShader(path, ST_Compute);

    PipelineStateObject* pso = new PipelineStateObject(name, signature, program);
    m_PSOs.Add(pso);
    return pso;
}

void RenderingContext::PreFrameUpdate()
{
    if (m_PendingShadersReloading)
    {
        m_ShaderCompilationResults.Clear();
        memset(static_cast<void*>(m_ShaderCompilationResultsBuffer), 0, ShaderCompilationResultsBufferSize);

        cstr8 string = m_ShaderCompilationResultsBuffer;

        for (ShaderProgram* program : m_ShaderPrograms)
        {
            bool compiled = program->Compile(DEBUG_BUILD || DEVELOPMENT_BUILD, string);

            if (!compiled)
            {
                m_ShaderCompilationResults.Add(string);
                string += strlen(string) + 1;
            }
        }

        for (PipelineStateObject* pso : m_PSOs)
        {
            pso->Recreate();
        }

        m_PendingShadersReloading = false;
    }

    ProcessUploads();
}

void RenderingContext::PostFrameUpdate()
{

}

void RenderingContext::Present()
{
    m_SwapChain->Present();
}

CommandList* RenderingContext::CreateCommandList(CommandListType type, ccstr8 name, i32 size)
{
    CommandList* list = new CommandList(type, name, size);
    m_Lists.Add(list);
    return list;
}

void RenderingContext::ProcessUploads()
{
    m_GraphicsCommandQueue->CPUWait();
    m_ComputeCommandQueue->CPUWait();
    m_CopyCommandQueue->CPUWait();

    m_CopyCommandQueue->BeginEvent("FramePreparation");
    m_GraphicsCommandQueue->BeginEvent("FramePreparation");

    m_GraphicsCommandList->Reset();

    for (u32 destination = 0; destination < UD_Count; ++destination)
    {
        Array<UploadRequest>& requests = m_UploadRequests[destination];
        for (const UploadRequest& request : requests)
        {
            m_GraphicsCommandList->Transition(*request.Destiantion, ResourceState::Common);
        }
    }

    m_GraphicsCommandList->Close();
    m_GraphicsCommandQueue->Execute(m_GraphicsCommandList);

    m_CopyCommandQueue->GPUWait(m_GraphicsCommandQueue, m_GraphicsCommandQueue->Signal());

    for (u32 destination = 0; destination < UD_Count; ++destination)
    {
        Array<UploadRequest>& requests = m_UploadRequests[destination];

        UploadRequestProcessor processor(requests, m_UploadBuffer);

        while (processor.HasUnprocessedRequests())
        {
            m_CopyCommandQueue->CPUWait();

            m_CopyCommandList->Reset();

            processor.CollectAndUpload();
            processor.Copy(m_CopyCommandList);
            
            m_CopyCommandList->Close();

            m_CopyCommandQueue->Execute(m_CopyCommandList);
        }
    }

    m_CopyCommandQueue->CPUWait();

    m_GraphicsCommandList->Reset();

    for (u32 destination = 0; destination < UD_Count; ++destination)
    {
        Array<UploadRequest>& requests = m_UploadRequests[destination];
        for (const UploadRequest& request : requests)
        {
            m_GraphicsCommandList->Transition(*request.Destiantion, request.State);
        }

        requests.Clear();
    }

    for (const TransitionRequest& request : m_TransitionRequests)
    {
        m_GraphicsCommandList->Transition(*request.View, request.State);
    }
    m_TransitionRequests.Clear();

    m_GraphicsCommandList->Close();
    m_GraphicsCommandQueue->Execute(m_GraphicsCommandList);

    m_GraphicsCommandQueue->EndEvent();
    m_CopyCommandQueue->EndEvent();
}

RenderingContext::RenderingContext()
{
    s_Context = this;
}

void RenderingContext::Initialize(Window* window)
{
    m_Window = window;

    m_Device = new Device();

    m_GraphicsCommandList = new CommandList(CommandListType::Direct, "MainGraphicsCommandList");
    m_ComputeCommandList = new CommandList(CommandListType::Compute, "MainComputeCommnadList");
    m_CopyCommandList = new CommandList(CommandListType::Copy, "MainCopyCommandList");

    m_GraphicsCommandQueue = new CommandQueue(CommandListType::Direct, true, "MainGraphicsCommandQueue");
    m_ComputeCommandQueue = new CommandQueue(CommandListType::Compute, true, "MainComputeCommandQueue");
    m_CopyCommandQueue = new CommandQueue(CommandListType::Copy, false, "MainCopyCommmandQueue");

    m_SwapChain = new SwapChain(m_GraphicsCommandQueue, window, PixelFormat::RGBA8F, window->GetWidth(), window->GetHeight());

    m_UploadBuffersHeap = new Heap(HeapType::Upload, HF_AllowOnlyBuffers, UploadHeapSize, Memory::BytesToKB * 64, false, "UploadHeap");
    m_RenderTargetsHeap = new Heap(HeapType::Default, HF_AllowOnlyRTAndDSTextures, RenderTargetsHeapSize, Memory::BytesToMB * 4, true, "RenderTargetsHeap");
    m_UAVTargetsHeap = new Heap(HeapType::Default, HF_AllowAllBuffersAndTextures, UAVTargetsHeapSize, Memory::BytesToMB * 4, true, "UAVTargetsHeap");
    m_BuffersHeap = new Heap(HeapType::Default, HF_AllowOnlyBuffers, BuffersHeapSize, Memory::BytesToKB * 64, true, "BuffersHeap");
    m_TexturesHeap = new Heap(HeapType::Default, static_cast<HeapFlags>(HF_DenyBuffers | HF_DenyRTAndDSTextures), TexturesHeapSize, Memory::BytesToKB * 64, true, "TexturesHeap");

    m_UploadBuffer = m_UploadBuffersHeap->CreateResource(RF_None, ResourceState::GenericRead, 1, UploadBufferSize, "UploadBuffer");

    Resource* geometryVertexPoolBuffer = m_BuffersHeap->CreateResource(RF_None, ResourceState::Common, 1, GeometryVertexPoolSize, "GeometryVertexPoolBuffer");
    m_GeometryVertexPool.Initialize(geometryVertexPoolBuffer);

    Resource* geometryIndexPoolBuffer = m_BuffersHeap->CreateResource(RF_None, ResourceState::Common, 1, GeometryIndexPoolSize, "GeometryIndexPoolBuffer");
    m_GeometryIndexPool.Initialize(geometryIndexPoolBuffer);

    m_CBVHeap = new DescriptorHeap(DescriptorHeapType::CBV, (1 << 16), true, "CBV_SRV_UAV_Heap");
    m_SRVHeap = m_CBVHeap;
    m_UAVHeap = m_CBVHeap;
    m_RTVHeap = new DescriptorHeap(DescriptorHeapType::RTV, (1 << 8), false, "RTVHeap");
    m_DSVHeap = new DescriptorHeap(DescriptorHeapType::DSV, (1 << 4), false, "DSVHeap");

    for (Resource* resource : m_SwapChain->GetTargets())
    {
        m_DisplayBuffersRTVs.Add(m_RTVHeap->CreateView(DescriptorHeapType::RTV, resource));
        m_DisplayBuffersSRVs.Add(m_SRVHeap->CreateView(DescriptorHeapType::SRV, resource));
    }

    m_ShaderCompilationResultsBuffer = Strings::RequestString(ShaderCompilationResultsBufferSize, false, true);
    m_PendingShadersReloading = false;
}

void RenderingContext::Deinitialize()
{
    for (RootSignature* signature : m_RootSignatures)
    {
        delete signature;
    }
    m_RootSignatures.Clear();

    delete m_DSVHeap;
    delete m_RTVHeap;
    //delete m_UAVHeap;
    //delete m_SRVHeap;
    delete m_CBVHeap;

    delete m_UAVTargetsHeap;
    delete m_RenderTargetsHeap;
    delete m_TexturesHeap;
    delete m_BuffersHeap;
    delete m_UploadBuffersHeap;

    delete m_SwapChain;

    delete m_CopyCommandQueue;
    delete m_ComputeCommandQueue;
    delete m_GraphicsCommandQueue;

    delete m_CopyCommandList;
    delete m_ComputeCommandList;
    delete m_GraphicsCommandList;

    for (CommandList* list : m_Lists)
    {
        delete list;
    }
    m_Lists.Clear();

    delete m_Device;
}

RenderingContext::UploadRequestProcessor::UploadRequestProcessor(const Array<UploadRequest>& requests, Resource* uploadBuffer)
    : m_Requests(requests), m_UploadBuffer(uploadBuffer), m_UploadBufferData(nullptr), m_Pointer(0), m_PendingPointer(0)
{
    m_UploadBuffer->Map(0, 0, m_UploadBuffer->GetSize(), reinterpret_cast<void**>(&m_UploadBufferData));
}

void RenderingContext::UploadRequestProcessor::CollectAndUpload()
{
    u64 size = 0;
    u64 capacity = m_UploadBuffer->GetSize();
    while (m_PendingPointer < m_Requests.GetSize())
    {
        const UploadRequest& request = m_Requests[m_PendingPointer];
        u64 requestSize = request.GetSize();

        ED_ASSERT(requestSize < capacity, "No deferred upload request can be bigger than upload buffer capacity at the moment.");

        if (size + requestSize >= capacity)
        {
            return;
        }

        void* uploadSection = m_UploadBufferData + size;
        memcpy(uploadSection, request.Data, requestSize);

        size = Align(size + requestSize, Memory::BytesToKB * 64);

        ++m_PendingPointer;
    }

    ED_LOG(RenderingContext, info, "Upload batch [{}\\{}]", size, capacity);
}

void RenderingContext::UploadRequestProcessor::Copy(CommandList* list)
{
    u64 offset = 0;
    for (u32 index = m_Pointer; index < m_PendingPointer; ++index)
    {
        const UploadRequest& request = m_Requests[index];

        ResourceView* view = request.Destiantion;

        Resource* destination = view->Viewed;

        list->Transition(destination, ResourceState::CopyDestination);

        switch (destination->GetType())
        {
        case Resource::BufferType:
        {
            list->CopyBufferRegion(m_UploadBuffer, offset, destination, request.Offset, request.GetSize());
            break;
        }
        case Resource::TextureType:
        {
            list->CopyTextureRegionFromBuffer(m_UploadBuffer, destination, offset, request.Width, request.ItemSize, 0, { 0, 0 });
            break;
        }
        default:
            ED_ASSERT(0, "Unsupported upload request destination resource type.")
            break;
        }

        list->Transition(destination, ResourceState::Common);

        view->Uploaded = 1;

        offset = Align(offset + request.GetSize(), Memory::BytesToKB * 64);
    }

    m_Pointer = m_PendingPointer;
}

RenderingContext::UploadRequestProcessor::~UploadRequestProcessor()
{
    m_UploadBuffer->Unmap(0, 0, m_UploadBuffer->GetSize());
}

ResourceView RenderingContext::GetDisplayBufferRTV() const
{
    return m_DisplayBuffersRTVs[m_SwapChain->GetActiveBackBufferIndex()];
}

ResourceView RenderingContext::GetDisplayBufferSRV() const
{
    return m_DisplayBuffersSRVs[m_SwapChain->GetActiveBackBufferIndex()];
}

void RenderingContext::RequsetDefferedTransition(const ResourceView& view, ResourceState state)
{
    TransitionRequest& request = m_TransitionRequests.Add();
    request.View = &view;
    request.State = state;
}

void RenderingContext::Wait()
{
    m_CopyCommandQueue->CPUWait();
    m_ComputeCommandQueue->CPUWait();
    m_GraphicsCommandQueue->CPUWait();
}

void RenderingContext::RequestShadersReloading()
{
    m_PendingShadersReloading = true;
}

RenderingContext::~RenderingContext()
{
    s_Context = nullptr;
}
